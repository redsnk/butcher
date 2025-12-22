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
	cpu->rsp.r64 = (uint64_t) (cpu->mem+(MEM_SIZE/2));
	cpu->rbp = cpu->rsp;
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

void check_stack(struct _cpu *cpu) {
uint8_t *p;

	p = (uint8_t *)(cpu->rsp.r64);
	if ((p < cpu->mem) && (p+8) > (cpu->mem + MEM_SIZE)) {
		panic("check_stat error","");
	}
}

void push(struct _cpu *cpu,int bits,uint64_t n) {
	switch (bits) {
		case 16:
			cpu->rsp.r64 -= 2;
			check_stack(cpu);
			*((uint16_t *)(cpu->rsp.r64)) = (uint16_t) n;
			break;
		case 32:
			cpu->rsp.r64 -= 4;
			check_stack(cpu);
			*((uint32_t *)(cpu->rsp.r64)) = (uint32_t) n;
			break;
		case 64:
			cpu->rsp.r64 -= 8;
			check_stack(cpu);
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
			ret = *((uint16_t *)(cpu->rsp.r64));
			cpu->rsp.r64 += 2;
			check_stack(cpu);
			break;
		case 32:
			ret = *((uint32_t *)(cpu->rsp.r64));
			cpu->rsp.r64 += 4;
			check_stack(cpu);
			break;
		case 64:
			ret = *((uint64_t *)(cpu->rsp.r64));
			cpu->rsp.r64 += 8;
			check_stack(cpu);
			break;
		default:
			panic("pop bits","");
	}
	return (ret);
}

uint64_t get_mem(struct _cpu *cpu,char *base,char *index,uint64_t mult,uint64_t disp) {
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
	mem = get_mem(cpu,base,index,mult,disp);
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
			break;
		case 16:
			*((uint16_t *)r) -= i;
			break;
		case 32:
			*((uint32_t *)r) -= i;
			break;
		case 64:
			*((uint64_t *)r) -= i;
			break;
		default:
			panic("sub_ri bits","");
	}
	print_cpu(cpu);
}


