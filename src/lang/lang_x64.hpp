#ifndef _LANG_X64_H
#define _LANG_X64_H

#include "language.hpp"

class Lang_x64 : public Language {
    public:
    /*
    const char *EQUAL = "";
    const char *NEQUAL = "";
    const char *GT = "";
    const char *LT = "";
    const char *GTE = "";
    const char *LTE = "";
    const char *F_SET_ZF = "";
    const char *F_SET_SF = "";
    const char *F_SET_CF = "";
    const char *F_SET_OF = "";
    const char *F_GET_ZF = "";
    const char *F_GET_SF = "";
    const char *F_GET_CF = "";
    const char *F_GET_OF = "";
    const char *F_ADD_OF = "";
    const char *F_ADD_CF = "";
    const char *F_NUM_CF = "";
    const char *F_SUB_OF = "";
    const char *F_IDIV = "";
    //const char *F_SUB_CF = "";
    const char *ENDF = "";
    const char *AND = "";
    const char *OR = "";
    const char *XOR = "";
    const char *SHL = "";
    const char *SHR = "";
    const char *F_TRUE = "";
    const char *F_FALSE = "";
    const char *F_PUSH = "";
    const char *F_POP = "";
    const char *F_POW = "";
    const char *E_JCC_GOTO = "";
    const char *E_IFTHENELSE = "";
    const char *E_IFTHEN = "";
    const char *F_NOT = "";
    const char *F_NEG = "";
    const char *F_PSHUFD = "";
    const char *E_NOT = "";
    const char *E_LABEL_NAME = "";
 
    const char *OP_ALONE = "";
    const char *OP_SUBNAME = "";
    const char *OP_REG = "";
    const char *OP_MEM = "";
    const char *OP_IMM = "";
    const char *E_CALL_FROM_IAT = "";
    const char *E_FUNC_NAME = "";
    const char *E_FUNC_ADDR = "";
    const char *E_RETURN = "";
    const char *E_GOTO = "";
    const char *E_LABEL = "";
    const char *E_JMP_FROM_IAT = "";
    const char *E_JE = "";
    const char *E_JNE = "";
    const char *E_JA = "";
    const char *E_JAE = "";
    const char *E_JL = "";
    const char *E_JLE = "";
    const char *E_JGE = "";
    const char *E_JG = "";
    const char *E_JO = "";
    const char *E_JNO = "";
    const char *E_JS = "";
    const char *E_PUSH = "";
    const char *E_POP = "";
    const char *E_SUB_RR = "";
    //const char *E_SUB_RI = "";
    const char *E_ADD_RR = "";
    const char *E_ADD_RI = "";
    const char *E_XOR_R = "";
    const char *E_XOR_RR = "";
    //const char *E_XOR_RI = "";
    const char *E_JNE_R_GOTO = "";
    const char *E_JE_R_GOTO = "";
    const char *E_JLE_R_GOTO = "";
    const char *E_JA_RR_GOTO = "";
    const char *E_JGE_RR_GOTO = "";
    const char *E_SPACE = "";
    const char *E_MOV_RR = "";
    const char *E_MOV_RI = "";
    const char *E_LEA_M = "";
    const char *E_MOV_RP = "";
    const char *E_MOV_RM = "";
    const char *E_MOV_PR = "";
    const char *E_MOV_MR = "";
    const char *E_MOV_PI = "";
    const char *E_MOV_MI = "";
    const char *E_STACK_INIT = "";
    const char *E_IF_R_EQ_I = "";
    const char *E_ELIF_R_EQ_I = "";
    const char *E_ENDIF = "";
    const char *E_GET_MEM = "";
    const char *E_S_GET_MEM = "";
    const char *E_LOAD_MEM = "";
    const char *E_SET_MEM = "";
    */

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
    virtual const char *F_IDIV(void) = 0;
    virtual const char *F_PUSHFPU(void) = 0;
    virtual const char *F_POPFPU(void) = 0;
    virtual const char *E_ANONJ(void) = 0;
    virtual const char *E_LABEL_ANONJ(void) = 0;

    virtual void PrintHeader(Code *c) = 0;
    virtual void PrintMainOpen(Code *c) = 0;
    virtual void PrintMainClose(Code *c) = 0;
    virtual void PrintSubMem(Code *c,int num) = 0;
    virtual void PrintFuncHeaderName(Code *c,int num,char *name) = 0;
    virtual void PrintFuncHeaderAddr(Code *c,int num) = 0;
    virtual void PrintAnonJmpVar(void) = 0;
    virtual void PrintAnonJmpCall(uint64_t addr,char *name) = 0;
    virtual void PrintFuncFooter(Code *c,int num) = 0;
    virtual void PrintSubCodeSep(void) = 0;
    //virtual char *mem_str(csh handle,cs_x86_op op) = 0;
    virtual char *reg_name(csh handle,int id_reg) = 0;
    virtual char *s_reg_name(csh handle,int id_reg) = 0;

    const char *ptr(cs_x86_op op);
    char *mem_str(csh handle,cs_x86_op op);
    char *op_str(csh handle,cs_x86_op op,int bits,int sign,int lset);
    char *get_op_str(csh handle,cs_x86_op op,int bits,int sign);
    char *set_op_str(csh handle,cs_x86_op op,int bits,int sign);

    char *Translate_reg (csh handle,cs_insn *insn,int reg8,int reg16,int reg32,int reg64,int sign);
    char *Translate_var (csh handle,cs_insn *insn,char *name, int lset);
    char *Translate_item (csh handle,cs_insn *insn,_s_item *i,int lset);
    char *Translate (csh handle,char *s, cs_insn *insn,int ends);
};

#endif // _LANG_X64_H
