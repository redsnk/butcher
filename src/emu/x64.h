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

#define MEM_SIZE		(1024*10)

struct _cpu {
    union _reg rax,rbx,rcx,rdx,r8,r9,r10,r11,r12,r13,r14,r15,rdi,rsi,rbp,rsp;
	uint8_t mem[MEM_SIZE];
};

void init(struct _cpu *cpu);
void push(struct _cpu *cpu,char *reg);
void lea(struct _cpu *cpu,char *rdst,char *rsrc,uint64_t disp);

#endif // _X64_H
