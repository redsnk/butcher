#ifndef _BUTCHER_X64_H
#define _BUTCHER_X64_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define FALSE 	(0)
#define TRUE	(-1)

#define MAX_PANIC	(256)

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

struct _s8 {
	int8_t l;
	int8_t h;
    int8_t l2;
    int8_t h2;
    int8_t l3;
    int8_t h3;
    int8_t l4;
    int8_t h4;
};

struct _r16 {
	uint16_t l;
	uint16_t h;
	uint16_t l2;
	uint16_t h2;
};

struct _s16 {
	int16_t l;
	int16_t h;
	int16_t l2;
	int16_t h2;
};

struct _r32 {
	uint32_t l;
	uint32_t h;
};

struct _s32 {
	int32_t l;
	int32_t h;
};

union _reg {
	uint64_t r64;
	int64_t s64;
	struct _r32 r32;
	struct _s32 s32;
	struct _r16 r16;
	struct _s16 s16;
	struct _r8 r8;
	struct _s8 s8;
};

union _xmm {
	__uint128_t r128;
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

union _freg {
	uint64_t u;
	double d;
};

struct _fpu {
	union _freg r[8];
	int top;
};

struct _mem {
	uint64_t addr;
	uint64_t size;
	uint64_t real_size;
	uint8_t *mem;
};

struct _cpu {
    union _reg rax,rbx,rcx,rdx,r8,r9,r10,r11,r12,r13,r14,r15,rdi,rsi,rbp,rsp;
	union _xmm xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;
	union _eflags eflags;
	struct _mem *mems;
	struct _fpu fpu;
	int mem_count;
	__int128_t tmp;
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

#define s_rax	(cpu->rax.s64)
#define s_rbx	(cpu->rbx.s64)
#define s_rcx	(cpu->rcx.s64)
#define s_rdx	(cpu->rdx.s64)
#define s_r8	(cpu->r8.s64)
#define s_r9	(cpu->r9.s64)
#define s_r10	(cpu->r10.s64)
#define s_r11	(cpu->r11.s64)
#define s_r12	(cpu->r12.s64)
#define s_r13	(cpu->r13.s64)
#define s_r14	(cpu->r14.s64)
#define s_r15	(cpu->r15.s64)
#define s_rsi	(cpu->rsi.s64)
#define s_rdi	(cpu->rdi.s64)
#define s_rbp	(cpu->rbp.s64)
#define s_rsp	(cpu->rsp.s64)

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

#define s_eax	(cpu->rax.s32.l)
#define s_ebx	(cpu->rbx.s32.l)
#define s_ecx	(cpu->rcx.s32.l)
#define s_edx	(cpu->rdx.s32.l)
#define s_r8d	(cpu->r8.s32.l)
#define s_r9d	(cpu->r9.s32.l)
#define s_r10d	(cpu->r10.s32.l)
#define s_r11d	(cpu->r11.s32.l)
#define s_r12d	(cpu->r12.s32.l)
#define s_r13d	(cpu->r13.s32.l)
#define s_r14d	(cpu->r14.s32.l)
#define s_r15d	(cpu->r15.s32.l)
#define s_esi	(cpu->rsi.s32.l)
#define s_edi	(cpu->rdi.s32.l)
#define s_ebp	(cpu->rbp.s32.l)
#define s_esp	(cpu->rsp.s32.l)

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

#define s_ax	(cpu->rax.s16.l)
#define s_bx	(cpu->rbx.s16.l)
#define s_cx	(cpu->rcx.s16.l)
#define s_dx	(cpu->rdx.s16.l)
#define s_r8w	(cpu->r8.s16.l)
#define s_r9w	(cpu->r9.s16.l)
#define s_r10w	(cpu->r10.s16.l)
#define s_r11w	(cpu->r11.s16.l)
#define s_r12w	(cpu->r12.s16.l)
#define s_r13w	(cpu->r13.s16.l)
#define s_r14w	(cpu->r14.s16.l)
#define s_r15w	(cpu->r15.s16.l)
#define s_si	(cpu->rsi.s16.l)
#define s_di	(cpu->rdi.s16.l)
#define s_bp	(cpu->rbp.s16.l)
#define s_sp	(cpu->rsp.s16.l)

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

#define s_al	(cpu->rax.s8.l)
#define s_bl	(cpu->rbx.s8.l)
#define s_cl	(cpu->rcx.s8.l)
#define s_dl	(cpu->rdx.s8.l)
#define s_ah	(cpu->rax.s8.h)
#define s_bh	(cpu->rbx.s8.h)
#define s_ch	(cpu->rcx.s8.h)
#define s_dh	(cpu->rdx.s8.h)
#define s_r8b	(cpu->r8.s8.l)
#define s_r9b	(cpu->r9.s8.l)
#define s_r10b	(cpu->r10.s8.l)
#define s_r11b	(cpu->r11.s8.l)
#define s_r12b	(cpu->r12.s8.l)
#define s_r13b	(cpu->r13.s8.l)
#define s_r14b	(cpu->r14.s8.l)
#define s_r15b	(cpu->r15.s8.l)
#define s_bpl	(cpu->rbp.s8.l)
#define s_spl	(cpu->rsp.s8.l)

#define _xmm0	(cpu->xmm0.r128)
#define _xmm1	(cpu->xmm1.r128)
#define _xmm2	(cpu->xmm2.r128)
#define _xmm3	(cpu->xmm3.r128)
#define _xmm4	(cpu->xmm4.r128)
#define _xmm5	(cpu->xmm5.r128)
#define _xmm6	(cpu->xmm6.r128)
#define _xmm7	(cpu->xmm7.r128)

#define _st0	cpu->fpu.r[cpu->fpu.top].d
#define _st1	cpu->fpu.r[(cpu->fpu.top+1)%8].d
#define _st2	cpu->fpu.r[(cpu->fpu.top+2)%8].d

#define _tmp 	(cpu->tmp)

#define _get_byte_ptr(m)	byte_ptr(cpu,m)
#define _get_word_ptr(m)	word_ptr(cpu,m)
#define _get_dword_ptr(m)	dword_ptr(cpu,m)
#define _get_qword_ptr(m)	qword_ptr(cpu,m)
#define _get_xword_ptr(m)	xword_ptr(cpu,m)
#define _get_dqword_ptr(m)	dqword_ptr(cpu,m)

#define s_get_byte_ptr(m)	s_byte_ptr(cpu,m)
#define s_get_word_ptr(m)	s_word_ptr(cpu,m)
#define s_get_dword_ptr(m)	s_dword_ptr(cpu,m)
#define s_get_qword_ptr(m)	s_qword_ptr(cpu,m)

#define _set_byte_ptr(m,v)		set_byte_ptr(cpu,m,v)
#define _set_word_ptr(m,v)		set_word_ptr(cpu,m,v)
#define _set_dword_ptr(m,v)		set_dword_ptr(cpu,m,v)
#define _set_qword_ptr(m,v)		set_qword_ptr(cpu,m,v)
#define _set_xword_ptr(m,v)		set_xword_ptr(cpu,m,v)
#define _set_dqword_ptr(m,v)	set_dqword_ptr(cpu,m,v)

void init(struct _cpu *cpu);
void end(struct _cpu *cpu);
//void panic(char *str1,char *str2);
void panic(const char *format,...);

void call_from_iat (struct _cpu *cpu,char *lib,char *func);
void jmp_from_iat (struct _cpu *cpu,char *lib,char *func);
uint8_t byte_ptr(struct _cpu *cpu,uint64_t addr);
uint16_t word_ptr(struct _cpu *cpu,uint64_t addr);
uint32_t dword_ptr(struct _cpu *cpu,uint64_t addr);
uint64_t qword_ptr(struct _cpu *cpu,uint64_t addr);
uint64_t xword_ptr(struct _cpu *cpu,uint64_t addr);
__uint128_t dqword_ptr(struct _cpu *cpu,uint64_t addr);
int8_t s_byte_ptr(struct _cpu *cpu,uint64_t addr);
int16_t s_word_ptr(struct _cpu *cpu,uint64_t addr);
int32_t s_dword_ptr(struct _cpu *cpu,uint64_t addr);
int64_t s_qword_ptr(struct _cpu *cpu,uint64_t addr);
void set_byte_ptr(struct _cpu *cpu,uint64_t addr,uint8_t value);
void set_word_ptr(struct _cpu *cpu,uint64_t addr,uint16_t value);
void set_dword_ptr(struct _cpu *cpu,uint64_t addr,uint32_t value);
void set_xword_ptr(struct _cpu *cpu,uint64_t addr,uint64_t value);
void set_qword_ptr(struct _cpu *cpu,uint64_t addr,uint64_t value);
void set_dqword_ptr(struct _cpu *cpu,uint64_t addr,__uint128_t value);
/*
void *get_reg(struct _cpu *cpu,char *reg,int *bits);
uint8_t *get_reg_8(struct _cpu *cpu,char *reg);
uint16_t *get_reg_16(struct _cpu *cpu,char *reg);
uint32_t *get_reg_32(struct _cpu *cpu,char *reg);
uint64_t *get_reg_64(struct _cpu *cpu,char *reg);
*/
int flag_z(struct _cpu *cpu);
int flag_c(struct _cpu *cpu);
//uint64_t num_flag_c(struct _cpu *cpu);
int flag_o(struct _cpu *cpu);
int flag_s(struct _cpu *cpu);
int flag_d(struct _cpu *cpu);
void set_flag_z(struct _cpu *cpu,int value);
void set_flag_s(struct _cpu *cpu,int value);
void set_flag_c(struct _cpu *cpu,int value);
void set_flag_o(struct _cpu *cpu,int value);
void set_flag_d(struct _cpu *cpu,int value);
void add_flag_c(struct _cpu *cpu,int bits,uint64_t op1,uint64_t op2);
void add_flag_o(struct _cpu *cpu,int bits,int64_t op1,int64_t op2);
//void sub_flag_c(struct _cpu *cpu,int bits,uint64_t op1,uint64_t op2);
void sub_flag_o(struct _cpu *cpu,int bits,int64_t op1,int64_t op2);
void push(struct _cpu *cpu,int b,uint64_t n);
uint64_t pop(struct _cpu *cpu,int b);
uint64_t Pow(uint64_t b,uint64_t p);
uint64_t neg(uint64_t b,uint64_t p);
uint64_t not(uint64_t b,uint64_t p);
uint64_t idiv(uint64_t a,uint64_t b);
__uint128_t pshufd (__uint128_t op1,uint8_t op2);
void pushfpu(struct _cpu *cpu,double v);
double popfpu(struct _cpu *cpu);
uint64_t mask(int bits);
double utod(uint64_t v);
uint64_t dtou(double v);

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
void op_rrr(struct _cpu *cpu,char *op,char *regd,char *regs,char *rege);
void op_rmi(struct _cpu *cpu,char *op,char *reg,char *base,char *index,uint64_t mult,uint64_t disp,uint64_t i);
void op_mm(struct _cpu *cpu,char *op,char *based,char *indexd,uint64_t multd,uint64_t dispd,char *bases,char *indexs,uint64_t mults,uint64_t disps);
void op_rrri(struct _cpu *cpu,char *op,char *regd,char *regs,char *rege,uint64_t i);

#define EXTRA_MEM	(1024)

void add_mem (struct _cpu *cpu,uint64_t addr,const char *mem,int size);
void sort_mem (struct _cpu *cpu);
int locate_mem (struct _cpu *cpu,uint64_t addr);
void add_zero_mem (struct _cpu *cpu,uint64_t addr,int size);
void get_mem (struct _cpu *cpu,uint64_t addr,int size,uint8_t *mem);
void load_mem (struct _cpu *cpu,char *name,uint64_t d_Offset,uint64_t d_Size,uint64_t v_Address,uint64_t v_Size);
void set_mem (struct _cpu *cpu,uint64_t addr,int size,uint8_t *mem);
void del_mem(struct _cpu *cpu,int n);

void dump_mem (struct _cpu *cpu,uint64_t addr,int size);
uint64_t get_free_chunk(struct _cpu *cpu,int size);
uint64_t alloc_mem (struct _cpu *cpu,int size);
uint64_t realloc_mem (struct _cpu *cpu,uint64_t addr,int size);
void free_mem (struct _cpu *cpu,uint64_t addr);
void set_unicode_ptr (struct _cpu *cpu,uint64_t addr,char *str);
char *get_unicode_ptr(struct _cpu *cpu,uint64_t addr);

#endif // _BUTCHER_X64_H
