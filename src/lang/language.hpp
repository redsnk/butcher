#ifndef _LANGUAGE_H
#define _LANGUAGE_H

#include "../code.hpp"

class Language {
    public:
    const char *COMM = "";
    int COMM_SEP = 0;
    const char *OP_ALONE = "";
    const char *OP_SUBNAME = "";
    const char *OP_REG = "";
    const char *OP_MEM = "";
    const char *OP_IMM = "";
    const char *E_CALL_FROM_IAT = "";
    const char *E_FUNC_NAME = "";
    const char *E_FUNC_ADDR = "";

    virtual void PrintHeader(Code *c) = 0;
    virtual void PrintMainOpen(Code *c) = 0;
    virtual void PrintMainClose(Code *c) = 0;
    virtual void PrintSubMem(Code *c,int num) = 0;
    virtual void PrintFuncHeaderName(Code *c,int num,char *name) = 0;
    virtual void PrintFuncHeaderAddr(Code *c,int num) = 0;
    virtual void PrintFuncFooter(Code *c,int num) = 0;
    virtual void PrintSubCodeSep(void) = 0;
    virtual char *mem_str(csh handle,cs_x86_op op,char *buffer) = 0;

    const char *reg_name(csh handle,int id_reg);
};

#endif // _LANGUAGE_H