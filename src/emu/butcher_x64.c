#include "butcher_x64.h"

void init(struct _cpu *cpu) {
	memset(cpu,0,sizeof(struct _cpu));
	cpu->mem_count = 0;
	cpu->fpu.top = 0;
}

void end(struct _cpu *cpu) {
	for (int n=0;n<cpu->mem_count;n++) {
		free(cpu->mems[n].mem);
	}
	free(cpu->mems);
}

/*
void panic(char *str1,char *str2) {
	printf("PANIC: %s - %s\n",str1,str2);
	exit(0);
}
*/

void panic(const char *format,...) {
char buffer[MAX_PANIC];

	va_list argptr;
    va_start(argptr, format);
    vsprintf(buffer,format, argptr);
    va_end(argptr);
	printf("PANIC: %s\n",buffer);
	exit(0);
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
		if ((addr >= cpu->mems[n].addr) && (addr < (cpu->mems[n].addr+cpu->mems[n].size))) {
			return (n);
		}
	}
	return (-1);
}

void add_mem (struct _cpu *cpu,uint64_t addr,const char *mem,int size) {
	if (!cpu->mem_count) {
		cpu->mems = (struct _mem *) malloc(sizeof(struct _mem));
	} else {
		cpu->mems = (struct _mem *) realloc(cpu->mems,sizeof(struct _mem)*(cpu->mem_count+1));
	}
	cpu->mems[cpu->mem_count].addr = addr;
	cpu->mems[cpu->mem_count].size = size;
	cpu->mems[cpu->mem_count].real_size = size+EXTRA_MEM;
	cpu->mems[cpu->mem_count].mem = (uint8_t *) malloc(size+EXTRA_MEM);
	if (mem == NULL) {
		memset(cpu->mems[cpu->mem_count].mem,0,size);
	} else {
		memcpy(cpu->mems[cpu->mem_count].mem,mem,size);
	}
	cpu->mem_count++;
	sort_mem(cpu);
}

void sort_mem (struct _cpu *cpu) {
int n;
int lexit;
struct _mem m;

	do {
		lexit = TRUE;
		for (n=0;n<(cpu->mem_count-1);n++) {
			if (cpu->mems[n].addr > cpu->mems[n+1].addr) {
				m = cpu->mems[n];
				cpu->mems[n] = cpu->mems[n+1];
				cpu->mems[n+1] = m;
 				lexit = FALSE;
				break;
			}
		}

	}
	while (!lexit);
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
						panic("load_mem-fread");
					}
				}
				else {
					panic("load_mem-fseek");
				}	
				fclose(f);
			}
			else {
				panic("load_mem-fopen");
			}
		}
		add_mem (cpu,v_Address,mem,v_Size);
		free(mem);
	}
	else {
		panic("load_mem-calloc");
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
	panic("get_mem error: 0x%" PRIx64 ":%li",addr,size);
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
	panic("set_mem error: 0x%" PRIx64 ":%li",addr,size);
}

void del_mem(struct _cpu *cpu,int n) {
int m;

	for (m=n;m<cpu->mem_count-1;m++) {
		cpu->mems[m] = cpu->mems[m+1];
	}
	cpu->mem_count--;
}

#define DUMP_LINE (16)

void hexdump (uint64_t off,void *addr,int len) {
char *p;
char hex[256];
char str[256];
int i,n;
char buffer[16];
char c;

	i = 0;
	n = 0;
	hex[0] = 0;
	str[0] = 0;
	while (i < len) {
		c = ((char *) addr)[i];
		sprintf(buffer," %02x",(unsigned char )c);
		strcat (hex,buffer);
		if ((c>=0x20) && (c<=0x7e)) {
			sprintf(buffer,"%c",c);
			strcat(str,buffer);
		}
		else {
			strcat(str,".");
		}
		i++;
		if ((i == len) || ((i % DUMP_LINE) == 0)) {
			while (i % DUMP_LINE) {
				strcat (hex,"   ");
				strcat (str," ");
				i++;
			}
			printf("0x%" PRIx64 " %s  %s\n",off,hex,str);
			off += DUMP_LINE;
			str[0] = 0;
			hex[0] = 0;
		}
	}

}

void dump_mem (struct _cpu *cpu,uint64_t addr,int size) {
char *mem;
int n;

	mem = (void *) malloc(size);
	n = get_mem_dump (cpu,addr,size,mem);
	if (n) {
		hexdump(addr,mem,n);
	}
	free(mem);
}

uint64_t get_free_chunk(struct _cpu *cpu,int size) {
uint64_t i_addr,e_addr;
int n;

	i_addr = 0x1000;
	for (n=0;n<cpu->mem_count;n++) {
		e_addr = i_addr+size+EXTRA_MEM-1;
		if (e_addr < cpu->mems[n].addr) {
			return (i_addr);
		}
		i_addr = cpu->mems[n].addr+cpu->mems[n].real_size;
	}
	return (i_addr);
}

uint64_t alloc_mem (struct _cpu *cpu,int size) {
uint64_t addr;

	addr = get_free_chunk(cpu,size);
	add_mem(cpu,addr,NULL,size);
	return (addr);
}

uint64_t realloc_mem (struct _cpu *cpu,uint64_t addr,int size) {
int n,m;
uint64_t ret;

	n = locate_mem(cpu,addr);
	if (n >= 0) {
		if (size >= cpu->mems[n].real_size) {
			ret = alloc_mem(cpu,size);
			m = locate_mem(cpu,ret);
			if (m >= 0) {
				set_mem (cpu,ret,cpu->mems[n].size,cpu->mems[n].mem);
				del_mem(cpu,n);
				return (ret);
			}
		}
		else {
			cpu->mems[n].size = size;
			return(addr);
		}
	}
	panic("realloc_mem error: 0x%" PRIx64 ":%li",addr,size);
}

void free_mem (struct _cpu *cpu,uint64_t addr) {
int n;

	n = locate_mem(cpu,addr);
	if (n >= 0) {
		del_mem(cpu,n);
	}
	else {
		panic("free_mem error: 0x%" PRIx64,addr);
	}
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

uint64_t xword_ptr(struct _cpu *cpu,uint64_t addr) {
uint64_t value;

	get_mem (cpu,addr,10,(uint8_t *)&value);
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

void set_xword_ptr(struct _cpu *cpu,uint64_t addr,uint64_t value) {
	set_mem (cpu,addr,10,(uint8_t *)&value);
}

void set_dqword_ptr(struct _cpu *cpu,uint64_t addr,__uint128_t value) {
	set_mem (cpu,addr,16,(uint8_t *)&value);
}

void set_unicode_ptr (struct _cpu *cpu,uint64_t addr,char *str) {
int n;

	for (n=0;n<strlen(str);n++) {
		set_word_ptr (cpu,addr+(n*2),str[n]);
	}
}

char *get_unicode_ptr(struct _cpu *cpu,uint64_t addr) {
int n;
char *str,c;

	str = (char *) malloc(1024);
	n = 0;
	while (TRUE) {
		c = byte_ptr(cpu,addr);
		if (c) {
			str[n++] = c;
			addr += 2;
		}
		else {
			break;
		}
	}
	str[n] = 0;
	return (str);
}

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

/*
uint64_t num_flag_c(struct _cpu *cpu) {
	return(cpu->eflags.CF?1:0);
}
*/

int flag_o(struct _cpu *cpu) {
	return(cpu->eflags.OF);
}

int flag_s(struct _cpu *cpu) {
	return(cpu->eflags.SF);
}

int flag_d(struct _cpu *cpu) {
	return(cpu->eflags.DF);
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

void set_flag_d(struct _cpu *cpu,int value) {
	cpu->eflags.DF = value;
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


void push(struct _cpu *cpu,int b,uint64_t n) {
	cpu->rsp.r64 -= b/8;
	set_mem(cpu,cpu->rsp.r64,b/8,(uint8_t *)&n);
}

uint64_t pop(struct _cpu *cpu,int b) {
uint64_t ret=0;

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

uint64_t idiv(uint64_t a,uint64_t b) {
	return(a/b);
}

__uint128_t pshufd (__uint128_t op1,uint8_t op2) {
__uint128_t r;
uint8_t c;

	r = 0;
	c = op2 & 0b00000011;
	r |= (op1 >> (32*c)) & 0xffffffff;
	c = (op2 & 0b00001100) >> 2;
	r |= (__uint128_t)((op1 >> (32*c)) & 0xffffffff)  << 32;
	c = (op2 & 0b00110000) >> 4;
	r |= (__uint128_t)((op1 >> (32*c)) & 0xffffffff) << 64;
	c = (op2 & 0b11000000) >> 6;
	r |= (__uint128_t)((op1 >> (32*c)) & 0xffffffff) << 96;
	return (r);
}

void pushfpu(struct _cpu *cpu,double v) {
	cpu->fpu.top--;
	if (cpu->fpu.top < 0) cpu->fpu.top = 7;
	cpu->fpu.r[cpu->fpu.top].d = v;
}

double popfpu(struct _cpu *cpu) {
uint64_t v;

	v = cpu->fpu.r[cpu->fpu.top].d;
	cpu->fpu.r[cpu->fpu.top].d = 0;
	cpu->fpu.top++;
	if (cpu->fpu.top > 7) cpu->fpu.top = 0;
	return (v);
}

double utod(uint64_t v) {
union _freg c;

	c.u = v;
	return (c.d);
}

uint64_t dtou(double v) {
union _freg c;

	c.d = v;
	return (c.u);
}

uint64_t mask(int bits) {
__uint128_t v;

	v = (((__uint128_t)1) << bits);
	v = v - 1;
	return (v);
}

void call_from_iat (struct _cpu *cpu,char *lib,char *func) {
	panic("call_from_iat not implemented - %s",func);
}

void jmp_from_iat (struct _cpu *cpu,char *lib,char *func) {
	panic("jmp_from_iat not implemented - %s",func);
}

void op(struct _cpu *cpu,char *op) {
	printf("%s\n",op);
	panic("op");
}

void op_r(struct _cpu *cpu,char *op,char *reg) {
	printf("%s %s\n",op,reg);
	panic("op_r");
}


void op_rr(struct _cpu *cpu,char *op,char *regd,char *regs) {
	printf("%s %s,%s\n",op,regd,regs);
	panic("op_rr");
}

void op_ri(struct _cpu *cpu,char *op,char *reg,uint64_t i) {
	printf("%s %s,0x%" PRIx64 "\n",op,reg,i);
	panic("op_ri");
}

void op_rm(struct _cpu *cpu,char *op,char *reg,char *base,char *index,uint64_t mult,uint64_t disp) {
	printf("%s %s,[%s+%s*%" PRIi64 "+0x%" PRIx64 "]\n",op,reg,base,index,mult,disp);
	panic("op_rm");
}

void op_mr(struct _cpu *cpu,char *op,char *base,char *index,uint64_t mult,uint64_t disp,char *reg) {
	printf("%s [%s+%s*%" PRIi64 "+0x%" PRIx64 "],%s\n",op,base,index,mult,disp,reg);
	panic("op_mr");
}

void op_mi(struct _cpu *cpu,char *op,char *base,char *index,uint64_t mult,uint64_t disp,uint64_t i) {
	printf("%s [%s+%s*%" PRIi64 "+0x%" PRIx64 "],0x%" PRIx64 "\n",op,base,index,mult,disp,i);
	panic("op_mi");
}

void op_m(struct _cpu *cpu,char *op,char *base,char *index,uint64_t mult,uint64_t disp) {
	printf("%s [%s+%s*%" PRIi64 "+0x%" PRIx64 "]\n",op,base,index,mult,disp);
	panic("op_m");
}

void op_i(struct _cpu *cpu,char *op,uint64_t i) {
	printf("%s 0x%" PRIx64 "\n",op,i);
	panic("op_i");
}

void op_rri(struct _cpu *cpu,char *op,char *regd,char *regs,uint64_t i) {
	printf("%s %s,%s,0x%" PRIx64 "\n",op,regd,regs,i);
	panic("op_rri");
}

void op_rrr(struct _cpu *cpu,char *op,char *regd,char *regs,char *rege) {
	printf("%s %s,%s,%s\n",op,regd,regs,rege);
	panic("op_rrr");
}

void op_rmi(struct _cpu *cpu,char *op,char *reg,char *base,char *index,uint64_t mult,uint64_t disp,uint64_t i) {
	printf("%s %s,[%s+%s*%" PRIi64 "+0x%" PRIx64 "],0x%" PRIx64 "\n",op,reg,base,index,mult,disp,i);
	panic("op_rmi");
}

void op_mm(struct _cpu *cpu,char *op,char *based,char *indexd,uint64_t multd,uint64_t dispd,char *bases,char *indexs,uint64_t mults,uint64_t disps) {
	printf("%s [%s+%s*%" PRIi64 "+0x%" PRIx64 "],[%s+%s*%" PRIi64 "+0x%" PRIx64 "]\n",op,based,indexd,multd,dispd,bases,indexs,mults,disps);
	panic("op_mm");
}

void op_rrri(struct _cpu *cpu,char *op,char *regd,char *regs,char *rege,uint64_t i) {
	printf("%s %s,%s,%s,0x%" PRIx64 "\n",op,regd,regs,rege,i);
	panic("op_rrri");
}
