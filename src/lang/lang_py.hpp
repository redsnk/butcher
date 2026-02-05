#ifndef _LANG_PY_H
#define _LANG_PY_H
#include "language.hpp"

class Lang_Py : public Language {
    public:
    void PrintHeader(Code *c);
    void PrintMainOpen(Code *c);
    void PrintMainClose(Code *c);
    void PrintSubMem(Code *c,int num);
    void PrintFuncHeaderName(Code *c,int num,char *name);
    void PrintFuncHeaderAddr(Code *c,int num);
    void PrintFuncFooter(Code *c,int num);
    void PrintSubCodeSep(void);
    char *mem_str(csh handle,cs_x86_op op);
    char *reg_name(csh handle,int id_reg);
    char *s_reg_name(csh handle,int id_reg);

    Lang_Py();
    
};

#endif // _LANG_PY_H