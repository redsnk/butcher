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

#define STACK_ADDR		0xF000
#define STACK_SIZE		(1024*10)

struct _mem {
	uint64_t addr;
	uint64_t size;
	uint8_t *mem;
};

struct _cpu {
    union _reg rax,rbx,rcx,rdx,r8,r9,r10,r11,r12,r13,r14,r15,rdi,rsi,rbp,rsp;
	union _eflags eflags;
	struct _mem *mems;
	int mem_count;
};

// 64
#define _rax	(cpu->rax.r64)
#define _rbx	(cpu->rbx.r64)
#define _rcx	(cpu->rcx.r64)
#define _rdx	(cpu->rdx.r64)
#define _r8		(cpu->r8.r64)
#define _r9		(cpu->r9.r64)
#define _r10	(cpu->r10.r64)
#define _r11	(cpu->r11.r64)
#define _r12	(cpu->r12.r64)
#define _r13	(cpu->r13.r64)
#define _r14	(cpu->r14.r64)
#define _r15	(cpu->r15.r64)
#define _rsi	(cpu->rsi.r64)
#define _rdi	(cpu->rdi.r64)
#define _rbp	(cpu->rbp.r64)
#define _rsp	(cpu->rsp.r64)

// 32
#define _eax	(cpu->rax.r32.l)
#define _ebx	(cpu->rbx.r32.l)
#define _ecx	(cpu->rcx.r32.l)
#define _edx	(cpu->rdx.r32.l)
#define _r8d	(cpu->r8.r32.l)
#define _r9d	(cpu->r9.r32.l)
#define _r10d	(cpu->r10.r32.l)
#define _r11d	(cpu->r11.r32.l)
#define _r12d	(cpu->r12.r32.l)
#define _r13d	(cpu->r13.r32.l)
#define _r14d	(cpu->r14.r32.l)
#define _r15d	(cpu->r15.r32.l)
#define _esi	(cpu->rsi.r32.l)
#define _edi	(cpu->rdi.r32.l)
#define _ebp	(cpu->rbp.r32.l)
#define _esp	(cpu->rsp.r32.l)

// 16
#define _ax		(cpu->rax.r16.l)
#define _bx		(cpu->rbx.r16.l)
#define _cx		(cpu->rcx.r16.l)
#define _dx		(cpu->rdx.r16.l)
#define _r8w	(cpu->r8.r16.l)
#define _r9w	(cpu->r9.r16.l)
#define _r10w	(cpu->r10.r16.l)
#define _r11w	(cpu->r11.r16.l)
#define _r12w	(cpu->r12.r16.l)
#define _r13w	(cpu->r13.r16.l)
#define _r14w	(cpu->r14.r16.l)
#define _r15w	(cpu->r15.r16.l)
#define _si		(cpu->rsi.r16.l)
#define _di		(cpu->rdi.r16.l)
#define _bp		(cpu->rbp.r16.l)
#define _sp		(cpu->rsp.r16.l)

// 8
#define _al		(cpu->rax.r8.l)
#define _bl		(cpu->rbx.r8.l)
#define _cl		(cpu->rcx.r8.l)
#define _dl		(cpu->rdx.r8.l)
#define _ah		(cpu->rax.r8.h)
#define _bh		(cpu->rbx.r8.h)
#define _ch		(cpu->rcx.r8.h)
#define _dh		(cpu->rdx.r8.h)
#define _r8b	(cpu->r8.r8.l)
#define _r9b	(cpu->r9.r8.l)
#define _r10b	(cpu->r10.r8.l)
#define _r11b	(cpu->r11.r8.l)
#define _r12b	(cpu->r12.r8.l)
#define _r13b	(cpu->r13.r8.l)
#define _r14b	(cpu->r14.r8.l)
#define _r15b	(cpu->r15.r8.l)
#define _bpl	(cpu->rbp.r8.l)
#define _spl	(cpu->rsp.r8.l)

#define _push(x)	op_r(cpu,"push",#x)
#define _pop(x)		op_r(cpu,"pop",#x)

#define _get_byte_ptr(m)	byte_ptr(cpu,m)
#define _get_word_ptr(m)	word_ptr(cpu,m)
#define _get_dword_ptr(m)	dword_ptr(cpu,m)
#define _get_qword_ptr(m)	qword_ptr(cpu,m)

#define _set_byte_ptr(m,v)	set_byte_ptr(cpu,m,v)
#define _set_word_ptr(m,v)	set_word_ptr(cpu,m,v)
#define _set_dword_ptr(m,v)	set_dword_ptr(cpu,m,v)
#define _set_qword_ptr(m,v)	set_qword_ptr(cpu,m,v)

void init(struct _cpu *cpu);
void end(struct _cpu *cpu);
void panic(char *str1,char *str2);
void add_mem (struct _cpu *cpu,uint64_t addr,const char *mem,int size);
void get_mem (struct _cpu *cpu,uint64_t addr,int size,uint8_t *mem);
void set_mem (struct _cpu *cpu,uint64_t addr,int size,uint8_t *mem);
void call_from_iat (char *lib,char *func);
uint8_t byte_ptr(struct _cpu *cpu,uint64_t addr);
uint16_t word_ptr(struct _cpu *cpu,uint64_t addr);
uint32_t dword_ptr(struct _cpu *cpu,uint64_t addr);
uint64_t qword_ptr(struct _cpu *cpu,uint64_t addr);
void set_byte_ptr(struct _cpu *cpu,uint64_t addr,uint8_t value);
void set_word_ptr(struct _cpu *cpu,uint64_t addr,uint16_t value);
void set_dword_ptr(struct _cpu *cpu,uint64_t addr,uint32_t value);
void set_qword_ptr(struct _cpu *cpu,uint64_t addr,uint64_t value);
void *get_reg(struct _cpu *cpu,char *reg,int *bits);
uint8_t *get_reg_8(struct _cpu *cpu,char *reg);
uint16_t *get_reg_16(struct _cpu *cpu,char *reg);
uint32_t *get_reg_32(struct _cpu *cpu,char *reg);
uint64_t *get_reg_64(struct _cpu *cpu,char *reg);
int flag_z(struct _cpu *cpu);
int flag_c(struct _cpu *cpu);

void op(struct _cpu *cpu,char *op);
void op_r(struct _cpu *cpu,char *op,char *reg);
void op_rr(struct _cpu *cpu,char *op,char *regd,char *regs);
void op_ri(struct _cpu *cpu,char *op,char *reg,uint64_t i);
void op_rm(struct _cpu *cpu,char *op,char *reg,char *base,char *index,uint64_t mult,uint64_t disp);
void op_mr(struct _cpu *cpu,char *op,char *base,char *index,uint64_t mult,uint64_t disp,char *reg);
void op_mi(struct _cpu *cpu,char *op,char *base,char *index,uint64_t mult,uint64_t disp,uint64_t i);
void op_m(struct _cpu *cpu,char *op,char *base,char *index,uint64_t mult,uint64_t disp);
void op_i(struct _cpu *cpu,char *op,uint64_t i);
void op_rri(struct _cpu *cpu,char *op,char *regd,char *regs,uint64_t i);
void op_mm(struct _cpu *cpu,char *op,char *based,char *indexd,uint64_t multd,uint64_t dispd,char *bases,char *indexs,uint64_t mults,uint64_t disps);
void op_rrri(struct _cpu *cpu,char *op,char *regd,char *regs,char *rege,uint64_t i);

#endif // _X64_H
