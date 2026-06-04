#ifndef _LANG_X64_H
#define _LANG_X64_H

#include "language.hpp"

class Lang_x64 : public Language {
    public:
    
    virtual const char *OP_ALONE(void) = 0;
    virtual const char *OP_SUBNAME(void) = 0;
    virtual const char *OP_REG(void) = 0;
    virtual const char *OP_IMM(void) = 0;
    virtual const char *OP_MEM(void) = 0;
    virtual const char *E_STACK_INIT(void) = 0;
    virtual const char *E_IF_R_EQ_I(void) = 0;
    virtual const char *E_ELIF_R_EQ_I(void) = 0;
    virtual const char *E_ENDIF(void) = 0;
    virtual const char *E_GET_MEM(void) = 0;
    virtual const char *E_S_GET_MEM(void) = 0;
    virtual const char *E_LOAD_MEM(void) = 0;
    virtual const char *E_SET_MEM(void) = 0;
    virtual const char *F_SET_ZF(void) = 0;
    virtual const char *F_SET_SF(void) = 0;
    virtual const char *F_SET_CF(void) = 0;
    virtual const char *F_SET_OF(void) = 0;
    virtual const char *F_GET_ZF(void) = 0;
    virtual const char *F_GET_SF(void) = 0;
    virtual const char *F_GET_CF(void) = 0;
    virtual const char *F_GET_OF(void) = 0;
    virtual const char *F_GET_DF(void) = 0;
    virtual const char *F_ADD_OF(void) = 0;
    virtual const char *F_ADD_CF(void) = 0;
    //virtual const char *F_NUM_CF(void) = 0;
    virtual const char *F_SUB_OF(void) = 0;
    virtual const char *F_SDIV(void) = 0;
    virtual const char *F_UDIV(void) = 0;
    virtual const char *F_PUSHFPU(void) = 0;
    virtual const char *F_POPFPU(void) = 0;
    virtual const char *E_ANONJ(void) = 0;
    virtual const char *E_LABEL_ANONJ(void) = 0;
    virtual const char *E_TMP(void) = 0;
    virtual const char *E_TMP2(void) = 0;
    virtual const char *F_UTOD(void) = 0;
    virtual const char *F_UTOF(void) = 0;
    virtual const char *F_DTOU(void) = 0;
    virtual const char *F_FTOU(void) = 0;

    virtual void PrintHeader(Code *c) = 0;
    virtual void PrintMainOpen(Code *c) = 0;
    virtual void PrintMainClose(Code *c,char *name) = 0;
    virtual void PrintSubMem(Code *c,int num) = 0;
    virtual void PrintFuncHeaderName(Code *c,int num,char *name) = 0;
    virtual void PrintFuncHeaderAddr(Code *c,int num) = 0;
    virtual void PrintAnonJmpVar(void) = 0;
    virtual void PrintAnonJmpCall(uint64_t addr,char *name) = 0;
    virtual void PrintAnonJmpEnd(void) = 0;
    virtual void PrintFuncFooter(Code *c,int num) = 0;
    virtual void PrintSubCodeSep(void) = 0;
    //virtual char *mem_str(csh handle,cs_x86_op op) = 0;
    virtual char *reg_name(csh handle,int id_reg) = 0;
    virtual char *s_reg_name(csh handle,int id_reg) = 0;

    const char *ptr(cs_x86_op op);
    char *mem_str(csh handle,cs_insn *insn,int nop);
    char *op_str(csh handle,cs_insn *insn,int nop,int bits,int sign,int lset);
    char *get_op_str(csh handle,cs_insn *insn,int nop,int bits,int sign);
    char *set_op_str(csh handle,cs_insn *insn,int nop,int bits,int sign);

    char *Translate_reg (csh handle,cs_insn *insn,int reg8,int reg16,int reg32,int reg64,int sign);
    char *Translate_var (csh handle,cs_insn *insn,char *name, int lset);
    char *Translate_item (csh handle,cs_insn *insn,_s_item *i,int lset);
    char *Translate (csh handle,const char *s, cs_insn *insn,int ends);
};

#define TEST_E      "get_zf()"
#define TEST_NE     "!get_zf()"
#define TEST_A      "(!get_cf()) & (!get_zf())"
#define TEST_AE     "!get_cf()"
#define TEST_L      "get_sf() != get_of()"
#define TEST_LE     "get_zf() | (get_sf() != get_of())"
#define TEST_G      "(!get_zf()) & (get_sf() == get_of())"
#define TEST_GE     "get_sf() == get_of()"
#define TEST_O     "get_of()"
#define TEST_NO    "!get_of()"
#define TEST_S     "get_sf()"
#define TEST_NS    "!get_sf()"
#define TEST_B      "get_cf()"
#define TEST_BE     "get_cf() | get_zf()"

#endif // _LANG_X64_H
