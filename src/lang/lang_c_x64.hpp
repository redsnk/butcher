#ifndef _LANG_C_X64_H
#define _LANG_C_X64_H

#include "lang_x64.hpp"

class Lang_C_x64 : public Lang_x64 {
    public:
    void PrintHeader(Code *c);
    void PrintAnonCalls(Code *c);
    void PrintMainOpen(Code *c);
    void PrintMainClose(Code *c,char *name);
    void PrintSubMem(Code *c,int num);
    void PrintFuncHeaderName(Code *c,int num,char *name);
    void PrintFuncHeaderAddr(Code *c,int num);
    void PrintAnonJmpVar(void);
    void PrintAnonJmpCall(uint64_t addr,char *name);
    void PrintAnonJmpEnd(void);
    void PrintFuncFooter(Code *c,int num);
    void PrintSubCodeSep(void);

    const char *COMM(void);
    int COMM_SEP(void);
    const char *INDENT(void);
    const char *ENDS(void);
    const char *E_LABEL(void);
    const char *E_LABEL_ANON(void);
    const char *E_CALL_FROM_IAT(void);
    const char *E_FUNC_NAME(void);
    const char *E_FUNC_ADDR(void);
    const char *E_RETURN(void);
    const char *E_GOTO(void);
    const char *E_JMP_FROM_IAT(void);
    const char *E_ENDIF(void);
    const char *E_JCC_GOTO(void);
    const char *E_SPACE(void);

    const char *OP_ALONE(void);
    const char *OP_SUBNAME(void);
    const char *OP_REG(void);
    const char *OP_IMM(void);
    const char *OP_MEM(void);

    const char *E_STACK_INIT(void);
    const char *E_IF_R_EQ_I(void);
    const char *E_ELIF_R_EQ_I(void);
    const char *E_GET_MEM(void);
    const char *E_S_GET_MEM(void);
    const char *E_LOAD_MEM(void);
    const char *E_SET_MEM(void);
    const char *F_SET_ZF(void);
    const char *F_SET_SF(void);
    const char *F_SET_CF(void);
    const char *F_SET_OF(void);
    const char *F_GET_ZF(void);
    const char *F_GET_SF(void);
    const char *F_GET_CF(void);
    const char *F_GET_OF(void);
    const char *F_GET_DF(void);
    const char *F_ADD_OF(void);
    const char *F_ADD_CF(void);
    //onst char *F_NUM_CF(void);
    const char *F_SUB_OF(void);
    const char *F_IDIV(void);

    const char *EQUAL(void);
    const char *NEQUAL(void);
    const char *GT(void);
    const char *LT(void);
    const char *GTE(void);
    const char *LTE(void);
    const char *ENDF(void);
    const char *AND(void);
    const char *OR(void);
    const char *XOR(void);
    const char *SHL(void);
    const char *SHR(void);
    const char *F_TRUE(void);
    const char *F_FALSE(void);
    const char *F_PUSH(void);
    const char *F_POP(void);
    const char *F_POW(void);
    const char *E_IFTHENELSE(void);
    const char *E_IFTHEN(void);
    const char *F_NOT(void);
    const char *F_NEG(void);
    const char *F_PSHUFD(void);
    const char *E_NOT(void);
    const char *E_LABEL_NAME(void);
    const char *E_LABEL_ADDR(void);
    const char *E_ANONC(void);
    const char *F_PUSHFPU(void);
    const char *F_POPFPU(void);
    const char *E_ANONJ(void);
    const char *E_LABEL_ANONJ(void);
    const char *E_WHILE(void);
    const char *E_BREAK(void);
    const char *E_TMP(void);
    const char *E_TMP2(void);
    const char *F_MASK(void);
    const char *F_UTOD(void);
    const char *F_DTOU(void);

    //char *mem_str(csh handle,cs_x86_op op);
    char *reg_name(csh handle,int id_reg);
    char *s_reg_name(csh handle,int id_reg);

    Lang_C_x64();
};

#endif // _LANG_C_X64_H
