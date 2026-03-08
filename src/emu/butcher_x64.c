#include "butcher_x64.h"

/*
void print_cpu(struct _cpu *cpu) {
	printf("------------------------------\n");
	printf("rbp:0x%016llx ",cpu->rbp.r64);
	printf("rsp:0x%016llx ",cpu->rsp.r64);
	printf("rax:0x%016llx ",cpu->rax.r64);
	printf("rbx:0x%016llx ",cpu->rbx.r64);
	printf("rcx:0x%016llx ",cpu->rcx.r64);
	printf("rdx:0x%016llx ",cpu->rdx.r64);
	printf("\n");
    printf("rdi:0x%016llx ",cpu->rdi.r64);
    printf("rsi:0x%016llx ",cpu->rsi.r64);
    printf(" r8:0x%016llx ",cpu->r8.r64);
    printf(" r9:0x%016llx ",cpu->r9.r64);
    printf("r10:0x%016llx ",cpu->r10.r64);
    printf("r11:0x%016llx ",cpu->r11.r64);
	printf("\n");
    printf("r12:0x%016llx ",cpu->r12.r64);
    printf("r13:0x%016llx ",cpu->r13.r64);
    printf("r14:0x%016llx ",cpu->r14.r64);
    printf("r15:0x%016llx ",cpu->r15.r64);
	printf("\n------------------------------\n");
}
*/

void init(struct _cpu *cpu) {
	memset(cpu,0,sizeof(struct _cpu));
	cpu->mem_count = 0;
	//add_mem(cpu,STACK_ADDR,NULL,STACK_SIZE);
	//cpu->rsp.r64 = (uint64_t) (STACK_ADDR+(STACK_SIZE/2));
	//cpu->rbp = cpu->rsp;
}

void end(struct _cpu *cpu) {
	for (int n=0;n<cpu->mem_count;n++) {
		free(cpu->mems[n].mem);
	}
	free(cpu->mems);
}

void panic(char *str1,char *str2) {
	printf("PANIC: %s - %s\n",str1,str2);
	exit(0);
}

void add_mem (struct _cpu *cpu,uint64_t addr,const char *mem,int size) {
	if (!cpu->mem_count) {
		cpu->mems = (struct _mem *) malloc(sizeof(struct _mem));
	} else {
		cpu->mems = (struct _mem *) realloc(cpu->mems,sizeof(struct _mem)*(cpu->mem_count+1));
	}
	cpu->mems[cpu->mem_count].addr = addr;
	cpu->mems[cpu->mem_count].size = size;
	cpu->mems[cpu->mem_count].mem = (uint8_t *) malloc(size);
	if (mem == NULL) {
		memset(cpu->mems[cpu->mem_count].mem,0,size);
	} else {
		memcpy(cpu->mems[cpu->mem_count].mem,mem,size);
	}
	cpu->mem_count++;
}

int locate_mem (struct _cpu *cpu,uint64_t addr) {
	for (int n=0;n<cpu->mem_count;n++) {
		if (cpu->mems[n].addr == addr) {
			return (n);
		}
	}
	return (-1);
}

int locate_addr_mem (struct _cpu *cpu,uint64_t addr) {
	for (int n=0;n<cpu->mem_count;n++) {
		if ((addr  >= cpu->mems[n].addr) && (addr < (cpu->mems[n].addr+cpu->mems[n].size))) {
			return (n);
		}
	}
	return (-1);
}

void load_mem (struct _cpu *cpu,char *name,uint64_t d_Offset,uint64_t d_Size,uint64_t v_Address,uint64_t v_Size) {
FILE *f;
char *mem;
uint64_t l;

	mem = calloc(1,v_Size);
	if (mem != NULL) {
		if (d_Size) {
			f = fopen(name,"r");
			if (f != NULL) {
				if (!fseek(f,d_Offset,SEEK_SET)) {
					l = fread(mem,1,d_Size,f);
					if (l != d_Size) {
						panic("load_mem","fread");
					}
				}
				else {
					panic("load_mem","fseek");
				}	
				fclose(f);
			}
			else {
				panic("load_mem","fopen");
			}
		}
		add_mem (cpu,v_Address,mem,v_Size);
		free(mem);
	}
	else {
		panic("load_mem","calloc");
	}
}

void get_mem (struct _cpu *cpu,uint64_t addr,int size,uint8_t *mem) {
	for (int n=0;n<cpu->mem_count;n++) {
		if ((addr >= cpu->mems[n].addr) && ((addr+size) <= (cpu->mems[n].addr+cpu->mems[n].size))) {
			if (mem != NULL) {
				int o = addr - cpu->mems[n].addr;
				memcpy (mem,cpu->mems[n].mem+o,size);
			}
			return;
		}
	}
	panic("get_mem","");
}

int get_mem_dump (struct _cpu *cpu,uint64_t addr,int size,uint8_t *mem) {
int n;
uint64_t last;

	n = locate_addr_mem (cpu,addr);
	if (n >= 0) {
		last = cpu->mems[n].addr + cpu->mems[n].size;
		if ((addr+size) > last) {
			size = last - addr;
		}
		get_mem(cpu,addr,size,mem);
		return (size);
	}
	return (0);
}

void set_mem (struct _cpu *cpu,uint64_t addr,int size,uint8_t *mem) {
	for (int n=0;n<cpu->mem_count;n++) {
		if ((addr >= cpu->mems[n].addr) && ((addr+size) <= (cpu->mems[n].addr+cpu->mems[n].size))) {
			int o = addr - cpu->mems[n].addr;
			memcpy (cpu->mems[n].mem+o,mem,size);
			return;
		}
	}
	panic("set_mem","");
}

void hexDump(uint64_t off,void *addr, int len) {
int i;
unsigned char buff[17];
unsigned char *pc = (unsigned char*)addr;

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                printf("  %s\n", buff);

            printf("  0x%016X ", off+i);
        }
		printf(" %02x", pc[i]);
		if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            buff[i % 16] = '.';
        } else {
            buff[i % 16] = pc[i];
        }
        buff[(i % 16) + 1] = '\0';
    }
    while ((i % 16) != 0) {
        printf("   ");
        i++;
    }
	printf("\n\n");
}

void dump_mem (struct _cpu *cpu,uint64_t addr,int size) {
char *mem;
int n;

	mem = (void *) malloc(size);
	n = get_mem_dump (cpu,addr,size,mem);
	if (n) {
		hexDump(addr,mem,n);
	}
	free(mem);
}

int unasigned_mem (struct _cpu *cpu,uint64_t addr,int size,uint64_t *next) {
int n;

	for (n=0;n<cpu->mem_count;n++) {
		if ((addr>=cpu->mems[n].addr) && (addr<(cpu->mems[n].addr+cpu->mems[n].size))) {
			*next = cpu->mems[n].addr+cpu->mems[n].size;
			return (FALSE);
		}
	}
	return (TRUE);
}

uint64_t alloc_mem (struct _cpu *cpu,int size) {
uint64_t res = 0x1000,next;

	while (!unasigned_mem(cpu,res,size,&next)) {
		res = next;
	}
	add_mem(cpu,res,NULL,size);
	return (res);
}

uint64_t realloc_mem (struct _cpu *cpu,uint64_t addr,int size) {
int n,m;
uint64_t ret;

	n = locate_mem(cpu,addr);
	if (n >= 0) {
		ret = alloc_mem(cpu,size);
		m = locate_mem(cpu,ret);
		if (m >= 0) {
			set_mem (cpu,ret,cpu->mems[n].size,cpu->mems[n].mem);
			free_mem(cpu,addr);
			return (ret);
		}
	}
	panic("realloc_mem","locate_mem");
}

void free_mem (struct _cpu *cpu,uint64_t addr) {
	// TODO
}

uint8_t byte_ptr(struct _cpu *cpu,uint64_t addr) {
uint8_t value;

	get_mem (cpu,addr,1,(uint8_t *)&value);
	return (value);
}

int8_t s_byte_ptr(struct _cpu *cpu,uint64_t addr) {
int8_t value;

	get_mem (cpu,addr,1,(uint8_t *)&value);
	return (value);
}

uint16_t word_ptr(struct _cpu *cpu,uint64_t addr) {
uint16_t value;

	get_mem (cpu,addr,2,(uint8_t *)&value);
	return (value);
}

int16_t s_word_ptr(struct _cpu *cpu,uint64_t addr) {
int16_t value;

	get_mem (cpu,addr,2,(uint8_t *)&value);
	return (value);
}

uint32_t dword_ptr(struct _cpu *cpu,uint64_t addr) {
uint32_t value;

	get_mem (cpu,addr,4,(uint8_t *)&value);
	return (value);
}

int32_t s_dword_ptr(struct _cpu *cpu,uint64_t addr) {
int32_t value;

	get_mem (cpu,addr,4,(uint8_t *)&value);
	return (value);
}

uint64_t qword_ptr(struct _cpu *cpu,uint64_t addr) {
uint64_t value;

	get_mem (cpu,addr,8,(uint8_t *)&value);
	return (value);
}

__uint128_t dqword_ptr(struct _cpu *cpu,uint64_t addr) {
__uint128_t value;

	get_mem (cpu,addr,16,(uint8_t *)&value);
	return (value);
}

int64_t s_qword_ptr(struct _cpu *cpu,uint64_t addr) {
int64_t value;

	get_mem (cpu,addr,8,(uint8_t *)&value);
	return (value);
}

void set_byte_ptr(struct _cpu *cpu,uint64_t addr,uint8_t value) {
	set_mem (cpu,addr,1,(uint8_t *)&value);
}

void set_word_ptr(struct _cpu *cpu,uint64_t addr,uint16_t value) {
	set_mem (cpu,addr,2,(uint8_t *)&value);
}

void set_dword_ptr(struct _cpu *cpu,uint64_t addr,uint32_t value) {
	set_mem (cpu,addr,4,(uint8_t *)&value);
}

void set_qword_ptr(struct _cpu *cpu,uint64_t addr,uint64_t value) {
	set_mem (cpu,addr,8,(uint8_t *)&value);
}

void set_dqword_ptr(struct _cpu *cpu,uint64_t addr,__uint128_t value) {
	set_mem (cpu,addr,16,(uint8_t *)&value);
}

/*
union _reg *get_reg64(struct _cpu *cpu,char *reg) {
// rax, eax, ax, ah, al
#define STRCMPREG(r)	!strcmp(reg,"r"#r"x") || !strcmp(reg,"e"#r"x") || !strcmp(reg,#r"x") || !strcmp(reg,#r"h") || !strcmp(reg,#r"l")
// rsp, esp, sp, spl
#define STRCMPREG2(r)    !strcmp(reg,"r"#r) || !strcmp(reg,"e"#r) || !strcmp(reg,#r) || !strcmp(reg,#r"l")
// r8, r8d, r8w, r8b
#define STRCMPREG3(r)    !strcmp(reg,#r) || !strcmp(reg,#r"d") || !strcmp(reg,#r"w") || !strcmp(reg,#r"b")

	if (STRCMPREG(a)) {
		return (&cpu->rax);
	}
	else if (STRCMPREG(b)) {
		return (&cpu->rbx);
	}
	else if (STRCMPREG(c)) {
		return (&cpu->rcx);
	}
	else if (STRCMPREG(d)) {
        return (&cpu->rdx);
    }
	else if (STRCMPREG2(sp)) {
        return (&cpu->rsp);
	}
	else if (STRCMPREG2(bp)) {
        return (&cpu->rbp);
    }
    else if (STRCMPREG2(si)) {
        return (&cpu->rsi);
    }
    else if (STRCMPREG2(di)) {
        return (&cpu->rdi);
    }
    else if (STRCMPREG3(r8)) {
        return (&cpu->r8);
    }
    else if (STRCMPREG3(r9)) {
        return (&cpu->r9);
    }
    else if (STRCMPREG3(r10)) {
        return (&cpu->r10);
    }
    else if (STRCMPREG3(r11)) {
        return (&cpu->r11);
    }
    else if (STRCMPREG3(r12)) {
        return (&cpu->rdi);
    }
    else if (STRCMPREG3(r13)) {
        return (&cpu->rdi);
    }
    else if (STRCMPREG3(r14)) {
        return (&cpu->rdi);
    }
    else if (STRCMPREG3(r15)) {
        return (&cpu->rdi);
    }
	else {
		panic("get_reg64 error",reg);
	}
	return (NULL);
}

void *get_reg(struct _cpu *cpu,char *reg,int *bits) {
union _reg *r;
char f,l;
void *ret;

	r = get_reg64(cpu,reg);
	if (r != NULL) {
		int len = strlen(reg);
		f = reg[0];
		l = reg[len-1];
		if ((f == 'r') && (l != 'd') && (l != 'w') && (l != 'b')) {
			if (bits != NULL) *bits = 64;
			return (&r->r64);
		}
		else if ((f == 'e') || (l == 'd')) {
			if (bits != NULL) *bits = 32;
			return (&r->r32.l);
		}
		else if (l == 'h') {
			if (bits != NULL) *bits = 8;
			return (&r->r8.h);
		}
		else if (l == 'l') { 
			if (bits != NULL) *bits = 8;
			return (&r->r8.l);
		}
		else {
			if (bits != NULL) *bits = 16;
			return (&r->r16.l);
		}
		return (r);

	}
	return (NULL);
}

uint8_t *get_reg_8(struct _cpu *cpu,char *reg) {
	return ((uint8_t *)get_reg(cpu,reg,NULL));
}
uint16_t *get_reg_16(struct _cpu *cpu,char *reg) {
	return ((uint16_t *)get_reg(cpu,reg,NULL));
}
uint32_t *get_reg_32(struct _cpu *cpu,char *reg) {
	return ((uint32_t *)get_reg(cpu,reg,NULL));
}
uint64_t *get_reg_64(struct _cpu *cpu,char *reg) {
	return ((uint64_t *)get_reg(cpu,reg,NULL));
}
*/
void set_z(struct _cpu *cpu,uint64_t i) {
	cpu->eflags.ZF = (i == 0);
}

void set_c(struct _cpu *cpu,uint64_t i) {
	cpu->eflags.CF = i;
}

int flag_z(struct _cpu *cpu) {
	return(cpu->eflags.ZF);
}

int flag_c(struct _cpu *cpu) {
	return(cpu->eflags.CF);
}

uint64_t num_flag_c(struct _cpu *cpu) {
	return(cpu->eflags.CF?1:0);
}

int flag_o(struct _cpu *cpu) {
	return(cpu->eflags.OF);
}

int flag_s(struct _cpu *cpu) {
	return(cpu->eflags.SF);
}

void set_flag_z(struct _cpu *cpu,int value) {
	cpu->eflags.ZF = value;
}

void set_flag_s(struct _cpu *cpu,int value) {
	cpu->eflags.SF = value;
}

void set_flag_c(struct _cpu *cpu,int value) {
	cpu->eflags.CF = value;
}

void set_flag_o(struct _cpu *cpu,int value) {
	cpu->eflags.OF = value;
}

#define SIGN(a)	(a<0)

void add_flag_o(struct _cpu *cpu,int bits,int64_t op1,int64_t op2) {
int32_t op1_32,op2_32;
int16_t op1_16,op2_16;
int8_t op1_8,op2_8;

	cpu->eflags.OF = 0;
	switch (bits) {
		case 8:
			op1_8 = (int8_t) op1;
			op2_8 = (int8_t) op2;
			if (SIGN(op1_8) == SIGN(op2_8)) {
				cpu->eflags.OF = (SIGN(op1_8+op2_8) != SIGN(op1_8));
			}
			break;
		case 16:
			op1_16 = (int16_t) op1;
			op2_16 = (int16_t) op2;
			if (SIGN(op1_16) == SIGN(op2_16)) {
				cpu->eflags.OF = (SIGN(op1_16+op2_16) != SIGN(op1_16));
			}
			break;
		case 32:
			op1_32 = (int32_t) op1;
			op2_32 = (int32_t) op2;
			if (SIGN(op1_32) == SIGN(op2_32)) {
				cpu->eflags.OF = (SIGN(op1_32+op2_32) != SIGN(op1_32));
			}
			break;
		case 64:
			if (SIGN(op1) == SIGN(op2)) {
				cpu->eflags.OF = (SIGN(op1+op2) != SIGN(op1));
			}
			break;
	}
}

void sub_flag_o(struct _cpu *cpu,int bits,int64_t op1,int64_t op2) {
int32_t op1_32,op2_32;
int16_t op1_16,op2_16;
int8_t op1_8,op2_8;

	cpu->eflags.OF = 0;
	switch (bits) {
		case 8:
			op1_8 = (int8_t) op1;
			op2_8 = (int8_t) op2;
			if (SIGN(op1_8) != SIGN(op2_8)) {
				cpu->eflags.OF = (SIGN(op1_8-op2_8) != SIGN(op1_8));
			}
			break;
		case 16:
			op1_16 = (int16_t) op1;
			op2_16 = (int16_t) op2;
			if (SIGN(op1_16) != SIGN(op2_16)) {
				cpu->eflags.OF = (SIGN(op1_16-op2_16) != SIGN(op1_16));
			}
			break;
		case 32:
			op1_32 = (int32_t) op1;
			op2_32 = (int32_t) op2;
			if (SIGN(op1_32) != SIGN(op2_32)) {
				cpu->eflags.OF = (SIGN(op1_32-op2_32) != SIGN(op1_32));
			}
			break;
		case 64:
			if (SIGN(op1) != SIGN(op2)) {
				cpu->eflags.OF = (SIGN(op1-op2) != SIGN(op1));
			}
			break;
	}
}

void add_flag_c(struct _cpu *cpu,int bits,uint64_t op1,uint64_t op2) {
__int128 t;

	switch(bits) {
		case 8:
			t = (__int128) (uint8_t) op1 + (__int128) (uint8_t) op2;
			cpu->eflags.CF = (t > 0xff);
			break;
		case 16:
			t = (__int128) (uint16_t) op1 + (__int128) (uint16_t) op2;
			cpu->eflags.CF = (t > 0xffff);
			break;
		case 32:
			t = (__int128) (uint32_t) op1 + (__int128) (uint32_t) op2;
			cpu->eflags.CF = (t > 0xffffffff);
			break;
		case 64:
			t = (__int128) (uint64_t) op1 + (__int128) (uint64_t) op2;
			cpu->eflags.CF = (t > 0xffffffffffffffff);
			break;
	}
}

char *get_mem_str (struct _cpu *cpu, uint64_t addr,int max) {
char *m;
int n;

	m = (char *) malloc(max);
	n = 0;
	for (n=0;n<(max);n++) {
		get_mem(cpu,addr+n,1,m+n);
		if (!m[n]) {
			break;
		}
	}
	return (m);
}

char *get_mem_uni16 (struct _cpu *cpu, uint64_t addr,int max) {
char *m;
int n;

	m = (char *) malloc(max);
	n = 0;
	for (n=0;n<(max);n+=2) {
		get_mem(cpu,addr+n,2,m+n);
		if (!m[n] && !m[n+1]) {
			break;
		}
	}
	return (m);
}

char *uni16_to_str(char *su) {
char *str;
int n;

	// TODO: max
	str = (char *) malloc(1024);
	n = 0;
	while (su[n] != 0) {
		str[n/2] = su[n];
		n += 2;
	}
	str[n/2] = 0;
	return (str);
}

/*
void sub_flag_c(struct _cpu *cpu,int bits,uint64_t op1,uint64_t op2) {
__int128 t;

	switch(bits) {
		case 8:
			t = (__int128) (uint8_t) op1 - (__int128) (uint8_t) op2;
			cpu->eflags.CF = (t > 0xff);
			break;
		case 16:
			t = (__int128) (uint16_t) op1 - (__int128) (uint16_t) op2;
			cpu->eflags.CF = (t > 0xffff);
			break;
		case 32:
			t = (__int128) (uint32_t) op1 - (__int128) (uint32_t) op2;
			cpu->eflags.CF = (t > 0xffffffff);
			break;
		case 64:
			t = (__int128) (uint64_t) op1 - (__int128) (uint64_t) op2;
			cpu->eflags.CF = (t > 0xffffffffffffffff);
			break;
	}
}
*/

void push(struct _cpu *cpu,int b,uint64_t n) {
	/*
	switch (bits) {
		case 8:
			cpu->rsp.r64 -= 1;
			set_mem(cpu,cpu->rsp.r64,1,(uint8_t *)&n);
			break;
		case 16:
			cpu->rsp.r64 -= 2;
			set_mem(cpu,cpu->rsp.r64,2,(uint8_t *)&n);
			break;
		case 32:
			cpu->rsp.r64 -= 4;
			set_mem(cpu,cpu->rsp.r64,4,(uint8_t *)&n);
			break;
		case 64:
			cpu->rsp.r64 -= 8;
			set_mem(cpu,cpu->rsp.r64,8,(uint8_t *)&n);
			break;
		default:
			panic("push bits","");
	}
	*/
	cpu->rsp.r64 -= b/8;
	set_mem(cpu,cpu->rsp.r64,b/8,(uint8_t *)&n);
}

uint64_t pop(struct _cpu *cpu,int b) {
uint64_t ret=0;

	/*
	switch (bits) {
		case 8:
			get_mem(cpu,cpu->rsp.r64,1,(uint8_t*) &ret);
			cpu->rsp.r64 += 1;
			break;
		case 16:
			get_mem(cpu,cpu->rsp.r64,2,(uint8_t*) &ret);
			cpu->rsp.r64 += 2;
			break;
		case 32:
			get_mem(cpu,cpu->rsp.r64,4,(uint8_t*) &ret);
			cpu->rsp.r64 += 4;
			break;
		case 64:
			get_mem(cpu,cpu->rsp.r64,8,(uint8_t*) &ret);
			cpu->rsp.r64 += 8;
			break;
		default:
			panic("pop bits","");
	}
	*/
	get_mem(cpu,cpu->rsp.r64,b/8,(uint8_t*) &ret);
	cpu->rsp.r64 += b/8;
	return (ret);
}

uint64_t Pow(uint64_t b,uint64_t p) {
uint64_t n,r;

	r = 1;
	for (n=0;n<p;n++) {
		r = r * b;
	}
	return (r);
}

uint64_t neg(uint64_t b,uint64_t p) {
	return ((~p)+(uint64_t)1);
}

uint64_t not(uint64_t b,uint64_t p) {
	return(~p);
}

__uint128_t pshufd (__uint128_t op1,uint8_t op2) {
__uint128_t r;
uint8_t c;

	r = 0;
	c = op2 & 0b00000011;
	r |= (op1 >> (32*c)) && 0xffffffff;
	c = (op2 & 0b00001100) >> 2;
	r |= (__uint128_t)((op1 >> (32*c)) && 0xffffffff)  << 32;
	c = (op2 & 0b00110000) >> 4;
	r |= (__uint128_t)((op1 >> (32*c)) && 0xffffffff) << 64;
	c = (op2 & 0b11000000) >> 6;
	r |= (__uint128_t)((op1 >> (32*c)) && 0xffffffff) << 96;
	return (r);
}

/*
uint64_t get_ins_mem(struct _cpu *cpu,char *base,char *index,uint64_t mult,uint64_t disp,int *bits) {
void *rb,*ri;
int bi;
uint64_t ret = 0;

	rb = get_reg(cpu,base,bits);
	if (strlen(index)) {
		ri = get_reg(cpu,index,&bi);
	}
	switch (*bits) {
		case 16:
			ret = *((uint16_t *)rb)+ disp;
			if (strlen(index)) {
				ret += *((uint16_t *)ri)*mult;
			}
			break;
		case 32:
			ret = *((uint32_t *)rb)+ disp;
			if (strlen(index)) {
				ret += *((uint32_t *)ri)*mult;
			}
			break;
		case 64:
			ret = *((uint64_t *)rb)+ disp;
			if (strlen(index)) {
				ret += *((uint64_t *)ri)*mult;
			}
			break;
		default:
			panic("get_ins_mem bits","");
	}
	return (ret);
}
*/

void call_from_iat (struct _cpu *cpu,char *lib,char *func) {
	panic("call_from_iat not implemented",func);
}

void jmp_from_iat (struct _cpu *cpu,char *lib,char *func) {
	panic("jmp_from_iat not implemented",func);
}

// ---------------------------------------------------------------------------------
/*
//TODO: The OF and CF flags are set to 0. The SF, ZF, and PF flags are set according to the result (see the “Operation” section above). The state of the AF flag is undefined.
#define OP_TEST(bits) \
void op_test_##bits(struct _cpu *cpu,uint##bits##_t d,uint##bits##_t s) {\
	uint##bits##_t r = d & s;			\
	set_z(cpu,r);						\
}

OP_TEST(8)
OP_TEST(16)
OP_TEST(32)
OP_TEST(64)

// ---------------------------------------------------------------------------------

// TODO: The CF, OF, SF, ZF, AF, and PF flags are set according to the result.
#define OP_CMP(bits) \
void op_cmp_##bits(struct _cpu *cpu,uint##bits##_t d,uint##bits##_t s) {\
	set_c(cpu,s > d);					\
	set_z(cpu,d - s);					\
}

OP_CMP(8)
OP_CMP(16)
OP_CMP(32)
OP_CMP(64)

// ---------------------------------------------------------------------------------

// TODO: The OF and CF flags are cleared; the SF, ZF, and PF flags are set according to the result. The state of the AF flag is undefined.
#define OP_XOR(bits) \
void op_xor_##bits(struct _cpu *cpu,uint##bits##_t d,uint##bits##_t s) {\
	set_z(cpu,d ^ s);					\
}

OP_XOR(8)
OP_XOR(16)
OP_XOR(32)
OP_XOR(64)

// ---------------------------------------------------------------------------------
*/
void op(struct _cpu *cpu,char *op) {
	printf("%s\n",op);
	panic("op",op);
	//print_cpu(cpu);
}

// ---------------------------------------------------------------------------------

/*
#define OP_R(bits) \
void op_r_##bits(struct _cpu *cpu,char *op,uint##bits##_t *r) {\
	if (!strcmp(op,"push")) {		\
		push(cpu,bits,*r);		\
	}								\
	else if(!strcmp(op,"pop")) {	\
		*r = pop(cpu,bits);		\
	}								\
	else {							\
		panic("op_r_"#bits" error",op);	\
	}								\
}

OP_R(8)
OP_R(16)
OP_R(32)
OP_R(64)
*/

void op_r(struct _cpu *cpu,char *op,char *reg) {
	printf("%s %s\n",op,reg);
/*
void *r;
int b;

	printf("%s %s\n",op,reg);
	r = get_reg(cpu,reg,&b);
	switch (b) {
		case 8:
			op_r_8(cpu,op,(uint8_t *)r);
			break;
		case 16:
			op_r_16(cpu,op,(uint16_t *)r);
			break;
		case 32:
			op_r_32(cpu,op,(uint32_t *)r);
			break;
		case 64:
			op_r_64(cpu,op,(uint64_t *)r);
			break;
		default:
			panic("op_r bits","");
	}
	print_cpu(cpu);
*/
	panic("op_r","");
}

// ---------------------------------------------------------------------------------
/*
#define OP_RR(bits) \
void op_rr_##bits(struct _cpu *cpu,char *op,uint##bits##_t *rd,uint##bits##_t *rs) {\
	if (!strcmp(op,"mov")) {		\
		*rd = *rs;					\
	}								\
	else if (!strcmp(op,"test")) {	\
		op_test_##bits(cpu,*rd,*rs);	\
	}								\
	else if (!strcmp(op,"xor")) {	\
		op_xor_##bits(cpu,*rd,*rs);	\
		*(rd) ^= *(rs);				\
	}								\
	else {							\
		panic("op_rr_"#bits" error",op);	\
	}								\
}

OP_RR(8)
OP_RR(16)
OP_RR(32)
OP_RR(64)
*/

void op_rr(struct _cpu *cpu,char *op,char *regd,char *regs) {
	printf("%s %s,%s\n",op,regd,regs);
/*
void *rd,*rs;
int bs,bd;

	printf("%s %s,%s\n",op,regd,regs);
	rd = get_reg(cpu,regd,&bd);
	rs = get_reg(cpu,regs,&bs);
	switch (bd) {
		case 8:
			op_rr_8(cpu,op,(uint8_t *)rd,(uint8_t *)rs);
			break;
		case 16:
			op_rr_16(cpu,op,(uint16_t *)rd,(uint16_t *)rs);
			break;
		case 32:
			op_rr_32(cpu,op,(uint32_t *)rd,(uint32_t *)rs);
			break;
		case 64:
			op_rr_64(cpu,op,(uint64_t *)rd,(uint64_t *)rs);
			break;
		default:
			panic("op_rr bits","");
	}
	print_cpu(cpu);	
*/
	panic("op_rr","");
}

// ---------------------------------------------------------------------------------
/*
#define OP_RI(bits) \
void op_ri_##bits(struct _cpu *cpu,char *op,uint##bits##_t *r,uint##bits##_t i) {\
	if (!strcmp(op,"mov") || !strcmp(op,"movabs")) {		\
		*r = i;						\
	}								\
	else if (!strcmp(op,"cmp")) {	\
		op_cmp_##bits(cpu,*r,i);	\
	}								\
	else if (!strcmp(op,"sub")) {	\
		op_cmp_##bits(cpu,*r,i);	\
		*r -= i;					\
	}								\
	else {							\
		panic("op_rr_"#bits" error",op);	\
	}								\
}

OP_RI(8)
OP_RI(16)
OP_RI(32)
OP_RI(64)
*/

void op_ri(struct _cpu *cpu,char *op,char *reg,uint64_t i) {
	printf("%s %s,0x%llx\n",op,reg,i);
/*
void *r;
int b;

	printf("%s %s,0x%llx\n",op,reg,i);
	r = get_reg(cpu,reg,&b);
	switch (b) {
		case 8:
			op_ri_8(cpu,op,(uint8_t *)r,(uint8_t) i);
			break;
		case 16:
			op_ri_16(cpu,op,(uint16_t *)r,(uint16_t) i);
			break;
		case 32:
			op_ri_32(cpu,op,(uint32_t *)r,(uint32_t) i);
			break;
		case 64:
			op_ri_64(cpu,op,(uint64_t *)r,(uint64_t) i);
			break;
		default:
			panic("op_ri bits","");
	}
	print_cpu(cpu);
*/
	panic("op_ri","");
}

// ---------------------------------------------------------------------------------
/*
#define OP_RM(bits) \
void op_rm_##bits(struct _cpu *cpu,char *op,uint##bits##_t *r,uint64_t mem) {\
	if (!strcmp(op,"mov")) {		\
		uint##bits##_t i;						\
		get_mem(cpu,mem,bits/8,(uint8_t *)&i);	\
		*r = i;	\
	}								\
	else if (!strcmp(op,"lea")) {	\
		*((uint##bits##_t *)r) = (uint##bits##_t) mem;	\
	}								\
	else {							\
		panic("op_rm_"#bits" error",op);	\
	}								\
}

OP_RM(8)
OP_RM(16)
OP_RM(32)
OP_RM(64)
*/

void op_rm(struct _cpu *cpu,char *op,char *reg,char *base,char *index,uint64_t mult,uint64_t disp) {
	printf("%s %s,[%s+%s*%i+0x%llx]\n",op,reg,base,index,mult,disp);
/*
void *r;
int b,bm;
uint64_t mem;

	printf("%s %s,[%s+%s*%i+0x%llx]\n",op,reg,base,index,mult,disp);
	r = get_reg(cpu,reg,&b);
	mem = get_ins_mem(cpu,base,index,mult,disp,&bm);
	switch (b) {
		case 16:
			op_rm_16(cpu,op,(uint16_t *)r,mem);
			break;
		case 32:
			op_rm_32(cpu,op,(uint32_t *)r,mem);
			break;
		case 64:
			op_rm_64(cpu,op,(uint64_t *)r,mem);
			break;
		default:
			panic("op_rm bits","");
	}
	print_cpu(cpu);
*/
	panic("op_rm","");
}

// ---------------------------------------------------------------------------------
/*
#define OP_MR(bits) \
void op_mr_##bits(struct _cpu *cpu,char *op,uint64_t mem,uint##bits##_t *r) {\
	if (!strcmp(op,"mov")) {				\
		set_mem(cpu,mem,bits/8,(uint8_t *)r);	\
	}										\
	else if (!strcmp(op,"cmp")) {			\
		uint##bits##_t i;					\
		get_mem(cpu,mem,bits/8,(uint8_t *)&i);	\
		op_cmp_##bits(cpu,i,*r);			\
	} 										\
	else {									\
		panic("op_mr_"#bits" error",op);	\
	}										\
}

OP_MR(8)
OP_MR(16)
OP_MR(32)
OP_MR(64)
*/

void op_mr(struct _cpu *cpu,char *op,char *base,char *index,uint64_t mult,uint64_t disp,char *reg) {
	printf("%s [%s+%s*%i+0x%llx],%s\n",op,base,index,mult,disp,reg);
/*
void *r;
int b,bm;
uint64_t mem;

	printf("%s [%s+%s*%i+0x%llx],%s\n",op,base,index,mult,disp,reg);
	r = get_reg(cpu,reg,&b);
	mem = get_ins_mem(cpu,base,index,mult,disp,&bm);
	switch (b) {
		case 8:
			op_mr_8(cpu,op,mem,(uint8_t *)r);
			break;
		case 16:
			op_mr_16(cpu,op,mem,(uint16_t *)r);
			break;
		case 32:
			op_mr_32(cpu,op,mem,(uint32_t *)r);
			break;
		case 64:
			op_mr_64(cpu,op,mem,(uint64_t *)r);
			break;
		default:
			panic("op_mr bits","");
	}
	print_cpu(cpu);
*/
	panic("op_mr","");
}

// ---------------------------------------------------------------------------------
/*
#define OP_MI(bits) \
void op_mi_##bits(struct _cpu *cpu,char *op,uint64_t mem,uint##bits##_t i) {\
	if (!strcmp(op,"mov")) {				\
		set_mem(cpu,mem,bits/8,(uint8_t *)&i);	\
	} 										\
	else {									\
		panic("op_mi_"#bits" error",op);	\
	}										\
}

OP_MI(8)
OP_MI(16)
OP_MI(32)
OP_MI(64)
*/

void op_mi(struct _cpu *cpu,char *op,char *base,char *index,uint64_t mult,uint64_t disp,uint64_t i) {
	printf("%s [%s+%s*%i+0x%llx],%llx\n",op,base,index,mult,disp,i);
/*
int b;
uint64_t mem;

	printf("%s [%s+%s*%i+0x%llx],%llx\n",op,base,index,mult,disp,i);
	mem = get_ins_mem(cpu,base,index,mult,disp,&b);
	switch (b) {
		case 8:
			op_mi_8(cpu,op,mem,(uint8_t)i);
			break;
		case 16:
			op_mi_16(cpu,op,mem,(uint16_t)i);
			break;
		case 32:
			op_mi_32(cpu,op,mem,(uint32_t)i);
			break;
		case 64:
			op_mi_64(cpu,op,mem,i);
			break;
		default:
			panic("op_mi bits","");
	}
	print_cpu(cpu);
*/
	panic("op_mi","");
}

// ---------------------------------------------------------------------------------
/*
#define OP_M(bits) \
void op_m_##bits(struct _cpu *cpu,char *op,uint64_t mem) {\
	panic("op_m_"#bits" error",op);	\
}

OP_M(8)
OP_M(16)
OP_M(32)
OP_M(64)
*/

void op_m(struct _cpu *cpu,char *op,char *base,char *index,uint64_t mult,uint64_t disp) {
	printf("%s [%s+%s*%i+0x%llx]\n",op,base,index,mult,disp);
/*
int b;
uint64_t mem;

	printf("%s [%s+%s*%i+0x%llx]\n",op,base,index,mult,disp);
	mem = get_ins_mem(cpu,base,index,mult,disp,&b);
	switch (b) {
		case 8:
			op_m_8(cpu,op,mem);
			break;
		case 16:
			op_m_16(cpu,op,mem);
			break;
		case 32:
			op_m_32(cpu,op,mem);
			break;
		case 64:
			op_m_64(cpu,op,mem);
			break;
		default:
			panic("op_m bits","");
	}
	print_cpu(cpu);
*/
	panic("op_m","");
}

// ---------------------------------------------------------------------------------
/*
#define OP_I(bits) \
void op_i_##bits(struct _cpu *cpu,char *op,uint64_t mem) {\
	panic("op_i_"#bits" error",op);	\
}

OP_I(8)
OP_I(16)
OP_I(32)
OP_I(64)
*/

void op_i(struct _cpu *cpu,char *op,uint64_t i) {
	printf("%s %llx\n",op,i);
/*
int b=0;

	printf("%s %llx\n",op,i);
	switch (b) {
		case 8:
			op_i_8(cpu,op,i);
			break;
		case 16:
			op_i_16(cpu,op,i);
			break;
		case 32:
			op_i_32(cpu,op,i);
			break;
		case 64:
			op_i_64(cpu,op,i);
			break;
		default:
			panic("op_i bits","");
	}
	print_cpu(cpu);
*/
	panic("op_i","");
}

// ---------------------------------------------------------------------------------
/*
#define OP_RRI(bits) \
void op_rri_##bits(struct _cpu *cpu,char *op,uint##bits##_t *rd,uint##bits##_t *rs,uint64_t i) {\
	panic("op_rri_"#bits" error",op);	\
}

OP_RRI(8)
OP_RRI(16)
OP_RRI(32)
OP_RRI(64)
*/

void op_rri(struct _cpu *cpu,char *op,char *regd,char *regs,uint64_t i) {
	printf("%s %s,%s,%llx\n",op,regd,regs,i);
/*
void *rd,*rs;
int bs,bd;

	printf("%s %s,%s,%llx\n",op,regd,regs,i);
	rd = get_reg(cpu,regd,&bd);
	rs = get_reg(cpu,regs,&bs);
	switch (bd) {
		case 8:
			op_rri_8(cpu,op,(uint8_t *)rd,(uint8_t *)rs,i);
			break;
		case 16:
			op_rri_16(cpu,op,(uint16_t *)rd,(uint16_t *)rs,i);
			break;
		case 32:
			op_rri_32(cpu,op,(uint32_t *)rd,(uint32_t *)rs,i);
			break;
		case 64:
			op_rri_64(cpu,op,(uint64_t *)rd,(uint64_t *)rs,i);
			break;
		default:
			panic("op_rri bits","");
	}
	print_cpu(cpu);	
*/
	panic("op_rri","");
}

void op_rrr(struct _cpu *cpu,char *op,char *regd,char *regs,char *rege) {
	printf("%s %s,%s,%s\n",op,regd,regs,rege);
	panic("op_rrr","");
}

void op_rmi(struct _cpu *cpu,char *op,char *reg,char *base,char *index,uint64_t mult,uint64_t disp,uint64_t i) {
	printf("%s %s,[%s+%s*%i+0x%llx],%llx\n",op,reg,base,index,mult,disp,i);
}

// ---------------------------------------------------------------------------------
/*
#define OP_MM(bits) \
void op_mm_##bits(struct _cpu *cpu,char *op,uint64_t memd,uint64_t mems) {\
	panic("op_mm_"#bits" error",op);	\
}

OP_MM(8)
OP_MM(16)
OP_MM(32)
OP_MM(64)
*/

void op_mm(struct _cpu *cpu,char *op,char *based,char *indexd,uint64_t multd,uint64_t dispd,char *bases,char *indexs,uint64_t mults,uint64_t disps) {
	printf("%s [%s+%s*%i+0x%llx],[%s+%s*%i+0x%llx]\n",op,based,indexd,multd,dispd,bases,indexs,mults,disps);
/*
int bd,bs;
uint64_t memd,mems;

	printf("%s [%s+%s*%i+0x%llx],[%s+%s*%i+0x%llx]\n",op,based,indexd,multd,dispd,bases,indexs,mults,disps);
	memd = get_ins_mem(cpu,based,indexd,multd,dispd,&bd);
	mems = get_ins_mem(cpu,bases,indexs,mults,disps,&bs);
	switch (bd) {
		case 8:
			op_mm_8(cpu,op,memd,mems);
			break;
		case 16:
			op_mm_16(cpu,op,memd,mems);
			break;
		case 32:
			op_mm_32(cpu,op,memd,mems);
			break;
		case 64:
			op_mm_64(cpu,op,memd,mems);
			break;
		default:
			panic("op_mm bits","");
	}
	print_cpu(cpu);
*/
	panic("op_mm","");
}

// ---------------------------------------------------------------------------------
/*
#define OP_RRRI(bits) \
void op_rrri_##bits(struct _cpu *cpu,char *op,uint##bits##_t *rd,uint##bits##_t *rs,uint##bits##_t *re,uint64_t i) {\
	panic("op_rrri_"#bits" error",op);	\
}

OP_RRRI(8)
OP_RRRI(16)
OP_RRRI(32)
OP_RRRI(64)
*/

void op_rrri(struct _cpu *cpu,char *op,char *regd,char *regs,char *rege,uint64_t i) {
	printf("%s %s,%s,%s,%llx\n",op,regd,regs,rege,i);
/*
void *rd,*rs,*re;
int bs,bd,be;

	printf("%s %s,%s,%s,%llx\n",op,regd,regs,rege,i);
	rd = get_reg(cpu,regd,&bd);
	rs = get_reg(cpu,regs,&bs);
	re = get_reg(cpu,rege,&be);
	switch (bd) {
		case 8:
			op_rrri_8(cpu,op,(uint8_t *)rd,(uint8_t *)rs,(uint8_t *)re,i);
			break;
		case 16:
			op_rrri_16(cpu,op,(uint16_t *)rd,(uint16_t *)rs,(uint16_t *)re,i);
			break;
		case 32:
			op_rrri_32(cpu,op,(uint32_t *)rd,(uint32_t *)rs,(uint32_t *)re,i);
			break;
		case 64:
			op_rrri_64(cpu,op,(uint64_t *)rd,(uint64_t *)rs,(uint64_t *)re,i);
			break;
		default:
			panic("op_rrri bits","");
	}
	print_cpu(cpu);	
*/
	panic("op_rrri","");
}
