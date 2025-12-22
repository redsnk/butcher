#ifndef _X64_H
#define _X64_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct _r8 {
	uint8_t l;
	uint8_t h;
    uint8_t l2;
    uint8_t h2;
    uint8_t l3;
    uint8_t h3;
    uint8_t l4;
    uint8_t h4;
};

struct _r16 {
	uint16_t l;
	uint16_t h;
	uint16_t l2;
	uint16_t h2;
};

struct _r32 {
	uint32_t l;
	uint32_t h;
};

union _reg {
	uint64_t r64;
	struct _r32 r32;
	struct _r16 r16;
	struct _r8 r8;
};

union _eflags {
  	struct {
    	uint32_t CF 	: 1;
    	uint32_t _u1 	: 1;
		uint32_t PF		: 1;
		uint32_t _u2 	: 1;
		uint32_t AF		: 1;
		uint32_t _u3 	: 1;
		uint32_t ZF		: 1;
		uint32_t SF		: 1;
		uint32_t TF		: 1;
		uint32_t IF		: 1;
		uint32_t DF		: 1;
		uint32_t OF		: 1;
  	};
  	uint32_t r32;
};

#define MEM_SIZE		(1024*10)

struct _cpu {
    union _reg rax,rbx,rcx,rdx,r8,r9,r10,r11,r12,r13,r14,r15,rdi,rsi,rbp,rsp;
	union _eflags eflags;
	uint8_t mem[MEM_SIZE];
};

void init(struct _cpu *cpu);
void push_r(struct _cpu *cpu,char *reg);
void pop_r(struct _cpu *cpu,char *reg);
void lea_rm(struct _cpu *cpu,char *reg,char *base,char *index,uint64_t mult,uint64_t disp);
void sub_ri(struct _cpu *cpu,char *reg,uint64_t i);

#endif // _X64_H
