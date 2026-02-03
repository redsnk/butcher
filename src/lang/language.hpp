#ifndef _LANGUAGE_H
#define _LANGUAGE_H

#include "../code.hpp"

enum Languages {
  C,
  PYTHON,
};

class Language {
    public:
    const char *COMM = "";
    int COMM_SEP = 0;
    const char *INDENT = "";
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
    const char *E_PUSH = "";
    const char *E_POP = "";
    const char *E_SUB_RR = "";
    const char *E_SUB_RI = "";
    const char *E_ADD_RR = "";
    const char *E_ADD_RI = "";
    const char *E_XOR_R = "";
    const char *E_XOR_RR = "";
    const char *E_XOR_RI = "";
    const char *E_JNE_GOTO = "";
    const char *E_JE_GOTO = "";
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
    

    virtual void PrintHeader(Code *c) = 0;
    virtual void PrintMainOpen(Code *c) = 0;
    virtual void PrintMainClose(Code *c) = 0;
    virtual void PrintSubMem(Code *c,int num) = 0;
    virtual void PrintFuncHeaderName(Code *c,int num,char *name) = 0;
    virtual void PrintFuncHeaderAddr(Code *c,int num) = 0;
    virtual void PrintFuncFooter(Code *c,int num) = 0;
    virtual void PrintSubCodeSep(void) = 0;
    virtual char *mem_str(csh handle,cs_x86_op op) = 0;
    virtual char *reg_name(csh handle,int id_reg) = 0;
};

#endif // _LANGUAGE_H