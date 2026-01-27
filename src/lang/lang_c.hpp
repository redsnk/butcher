#ifndef _LANG_C_H
#define _LANG_C_H
#include "language.hpp"

class Lang_C : public Language {
    public:
    void PrintHeader(Code *c);
    void PrintMainOpen(Code *c);
    void PrintMainClose(Code *c);
    void PrintSubMem(Code *c,int num);
    void PrintFuncHeaderName(Code *c,int num,char *name);
    void PrintFuncHeaderAddr(Code *c,int num);
    void PrintFuncFooter(Code *c,int num);
    void PrintSubCodeSep(void);
    char *mem_str(csh handle,cs_x86_op op,char *buffer);

    Lang_C();
};

#endif // _LANG_C_H