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

union _reg *get_reg64(struct _cpu *cpu,char *reg) {
#define STRCMPREG(r)	!strcmp(reg,"r"#r"x") || !strcmp(reg,"e"#r"x") || !strcmp(reg,#r"x") || !strcmp(reg,#r"h") || !strcmp(reg,#r"l")
#define STRCMPREG2(r)    !strcmp(reg,"r"#r) || !strcmp(reg,"e"#r) || !strcmp(reg,#r) || !strcmp(reg,#r"l")
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
		printf("get_reg64 error: %s\n",reg);
		exit(1);
	}
	return (NULL);
}

void check_stack(struct _cpu *cpu) {
uint8_t *p;

	p = (uint8_t *)(cpu->rsp.r64);
	if ((p < cpu->mem) && (p+8) > (cpu->mem + MEM_SIZE)) {
		printf("check_stat error\n");
		exit(1);
	}
}

void push64(struct _cpu *cpu,uint64_t n) {
	cpu->rsp.r64 -= 8;
	check_stack(cpu);
	*((uint64_t *)(cpu->rsp.r64)) = n;
}

// ---------------------------------------------

void push(struct _cpu *cpu,char *reg) {
union _reg *r;

	printf("push %s\n",reg);
	r = get_reg64(cpu,reg);
	push64(cpu,r->r64);
	print_cpu(cpu);
}

void lea(struct _cpu *cpu,char *rdst,char *rsrc,uint64_t disp) {
union _reg *rs,*rd;

	printf("lea %s,[%s+0x%llx]\n");
	rs = get_reg64(cpu,rsrc);
	rd = get_reg64(cpu,rdst);
	rd->r64 = rs->r64 + disp;
	print_cpu(cpu);
}

