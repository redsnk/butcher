#include "x64.h"

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

void init(struct _cpu *cpu) {
	memset(cpu,0,sizeof(struct _cpu));
	cpu->mem_count = 0;
	add_mem(cpu,STACK_ADDR,NULL,STACK_SIZE);
	cpu->rsp.r64 = (uint64_t) (STACK_ADDR+(STACK_SIZE/2));
	cpu->rbp = cpu->rsp;
}

void end(struct _cpu *cpu) {
	for (int n=0;n<cpu->mem_count;n++) {
		free(cpu->mems[n].mem);
	}
	free(cpu->mems);
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

void check_ptr (struct _cpu *cpu,uint64_t addr,int size) {
	get_mem(cpu,addr,size,NULL);
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

void set_mem (struct _cpu *cpu,uint64_t addr,int size,uint8_t *mem) {
	for (int n=0;n<cpu->mem_count;n++) {
		if ((addr >= cpu->mems[n].addr) && ((addr+size) <= (cpu->mems[n].addr+cpu->mems[n].size))) {
			int o = addr - cpu->mems[n].addr;
			memcpy (cpu->mems[n].mem+o,mem,size);
			return;
		}
	}
	panic("get_mem","");
}

uint64_t qword_ptr(struct _cpu *cpu,uint64_t addr) {
uint64_t value;

	get_mem (cpu,addr,8,(uint8_t *)&value);
	return (value);
}

void panic(char *str1,char *str2) {
	printf("PANIC: %s - %s\n",str1,str2);
	exit(0);
}

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
			*bits = 64;
			return (&r->r64);
		}
		else if ((f == 'e') || (l == 'd')) {
			*bits = 32;
			return (&r->r32.l);
		}
		else if (l == 'h') {
			*bits = 8;
			return (&r->r8.h);
		}
		else if (l == 'l') { 
			*bits = 8;
			return (&r->r8.l);
		}
		else {
			*bits = 16;
			return (&r->r16.l);
		}
		return (r);

	}
	return (NULL);
}

void set_z(struct _cpu *cpu,uint64_t i) {
	cpu->eflags.ZF = (i == 0);
}

int get_z(struct _cpu *cpu) {
	return(cpu->eflags.ZF);
}

void push(struct _cpu *cpu,int bits,uint64_t n) {
	switch (bits) {
		case 16:
			cpu->rsp.r64 -= 2;
			check_ptr(cpu,cpu->rsp.r64,2);
			*((uint16_t *)(cpu->rsp.r64)) = (uint16_t) n;
			break;
		case 32:
			cpu->rsp.r64 -= 4;
			check_ptr(cpu,cpu->rsp.r64,4);
			*((uint32_t *)(cpu->rsp.r64)) = (uint32_t) n;
			break;
		case 64:
			cpu->rsp.r64 -= 8;
			check_ptr(cpu,cpu->rsp.r64,8);
			*((uint64_t *)(cpu->rsp.r64)) = n;
			break;
		default:
			panic("push bits","");
	}
}

uint64_t pop(struct _cpu *cpu,int bits) {
uint64_t ret;

	switch (bits) {
		case 16:
			check_ptr(cpu,cpu->rsp.r64,2);
			ret = *((uint16_t *)(cpu->rsp.r64));
			cpu->rsp.r64 += 2;
			break;
		case 32:
			check_ptr(cpu,cpu->rsp.r64,4);
			ret = *((uint32_t *)(cpu->rsp.r64));
			cpu->rsp.r64 += 4;
			break;
		case 64:
			check_ptr(cpu,cpu->rsp.r64,8);
			ret = *((uint64_t *)(cpu->rsp.r64));
			cpu->rsp.r64 += 8;
			break;
		default:
			panic("pop bits","");
	}
	return (ret);
}

uint64_t get_ins_mem(struct _cpu *cpu,char *base,char *index,uint64_t mult,uint64_t disp) {
void *rb,*ri;
int bb,bi;
uint64_t ret = 0;

	rb = get_reg(cpu,base,&bb);
	ri = get_reg(cpu,index,&bi);
	switch (bb) {
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
			panic("get_mem bits","");
	}
	return (ret);
}

// ---------------------------------------------

void push_r(struct _cpu *cpu,char *reg) {
void *r;
int b;

	printf("push %s\n",reg);
	r = get_reg(cpu,reg,&b);
	switch (b) {
		case 16:
			push (cpu,16,*((uint16_t *)r));
			break;
		case 32:
			push (cpu,32,*((uint32_t *)r));
			break;
		case 64:
			push (cpu,64,*((uint64_t *)r));;
			break;
		default:
			panic("push_r bits","");
	}
	print_cpu(cpu);
}

void pop_r(struct _cpu *cpu,char *reg) {
void *r;
int b;

	printf("pop %s\n",reg);
	r = get_reg(cpu,reg,&b);
	switch (b) {
		case 16:
			*((uint16_t *)r) = (uint16_t) pop (cpu,16);
			break;
		case 32:
			*((uint32_t *)r) = (uint32_t) pop (cpu,32);
			break;
		case 64:
			*((uint64_t *)r) = pop (cpu,64);
			break;
		default:
			panic("pop_r bits","");
	}
	print_cpu(cpu);
}

void lea_rm(struct _cpu *cpu,char *reg,char *base,char *index,uint64_t mult,uint64_t disp) {
void *r;
int b;
uint64_t mem;

	printf("lea %s,[%s+%s*%i+0x%llx]\n",reg,base,index,mult,disp);
	r = get_reg(cpu,reg,&b);
	mem = get_ins_mem(cpu,base,index,mult,disp);
	switch (b) {
		case 16:
			*((uint16_t *)r) = (uint16_t) mem;
			break;
		case 32:
			*((uint32_t *)r) = (uint32_t) mem;
			break;
		case 64:
			*((uint64_t *)r) = mem;
			break;
		default:
			panic("lea_rm bits","");
	}
	print_cpu(cpu);
}

void sub_ri(struct _cpu *cpu,char *reg,uint64_t i) {
void *r;
int b;

	// TODO: The OF, SF, ZF, AF, PF, and CF flags are set according to the result.
	printf("sub %s,0x%llx\n",reg,i);
	r = get_reg(cpu,reg,&b);
	switch (b) {
		case 8:
			*((uint8_t *)r) -= i;
			set_z(cpu,*((uint8_t *)r));
			break;
		case 16:
			*((uint16_t *)r) -= i;
			set_z(cpu,*((uint16_t *)r));
			break;
		case 32:
			*((uint32_t *)r) -= i;
			set_z(cpu,*((uint32_t *)r));
			break;
		case 64:
			*((uint64_t *)r) -= i;
			set_z(cpu,*((uint64_t *)r));
			break;
		default:
			panic("sub_ri bits","");
	}
	print_cpu(cpu);
}

void xor_rr(struct _cpu *cpu,char *regd,char *regs) {
void *rd,*rs;
int bs,bd;

	printf("xor %s,%s\n",regd,regs);
	rd = get_reg(cpu,regd,&bd);
	rs = get_reg(cpu,regs,&bs);
	switch (bd) {
		case 8:
			*((uint8_t *)rd) ^= *((uint8_t *)rs);
			break;
		case 16:
			*((uint16_t *)rd) ^= *((uint16_t *)rs);
			break;
		case 32:
			*((uint32_t *)rd) ^= *((uint32_t *)rs);
			break;
		case 64:
			*((uint64_t *)rd) ^= *((uint64_t *)rs);
			break;
		default:
			panic("xor_rr bits","");
	}
	print_cpu(cpu);	
}

void mov_rr(struct _cpu *cpu,char *regd,char *regs) {
void *rd,*rs;
int bs,bd;

	printf("mov %s,%s\n",regd,regs);
	rd = get_reg(cpu,regd,&bd);
	rs = get_reg(cpu,regs,&bs);
	switch (bd) {
		case 8:
			*((uint8_t *)rd) = *((uint8_t *)rs);
			break;
		case 16:
			*((uint16_t *)rd) = *((uint16_t *)rs);
			break;
		case 32:
			*((uint32_t *)rd) = *((uint32_t *)rs);
			break;
		case 64:
			*((uint64_t *)rd) = *((uint64_t *)rs);
			break;
		default:
			panic("mov_rr bits","");
	}
	print_cpu(cpu);	
}

void mov_mr(struct _cpu *cpu,char *base,char *index,uint64_t mult,uint64_t disp,char *reg) {
void *r;
int b;
uint64_t mem;

	printf("mov [%s+%s*%i+0x%llx],%s\n",base,index,mult,disp,reg);
	r = get_reg(cpu,reg,&b);
	mem = get_ins_mem(cpu,base,index,mult,disp);
	switch (b) {
		case 16:
			set_mem(cpu,mem,2,(uint8_t *)r);
			break;
		case 32:
			set_mem(cpu,mem,4,(uint8_t *)r);
			break;
		case 64:
			set_mem(cpu,mem,8,(uint8_t *)r);
			break;
		default:
			panic("mov_mr bits","");
	}
	print_cpu(cpu);	
}

void mov_mi(struct _cpu *cpu,char *base,char *index,uint64_t mult,uint64_t disp,int64_t i) {
void *r;
int b;
uint64_t mem;
uint16_t i16;
uint32_t i32;

	printf("mov [%s+%s*%i+0x%llx],0x%llx\n",base,index,mult,disp,i);
	r = get_reg(cpu,base,&b);
	mem = get_ins_mem(cpu,base,index,mult,disp);
	switch (b) {
		case 16:
			i16 = i;
			set_mem(cpu,mem,2,(uint8_t *)&i16);
			break;
		case 32:
			i32 = i;
			set_mem(cpu,mem,4,(uint8_t *)&i32);
			break;
		case 64:
			set_mem(cpu,mem,8,(uint8_t *)&i);
			break;
		default:
			panic("mov_mi bits","");
	}
	print_cpu(cpu);	
}


