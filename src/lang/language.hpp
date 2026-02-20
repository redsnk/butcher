#ifndef _LANGUAGE_H
#define _LANGUAGE_H

#include "../code.hpp"
#include "emit.hpp"
#include "scanner.hpp"
#include "parser.hpp"

enum Languages {
  C,
  PYTHON,
};

class Language {
    public:
    const char *COMM = "";
    int COMM_SEP = 0;
    const char *INDENT = "";
    const char *ENDS = "";
    const char *SEPS = "";
    
    virtual void PrintHeader(Code *c) = 0;
    virtual void PrintMainOpen(Code *c) = 0;
    virtual void PrintMainClose(Code *c) = 0;
    virtual void PrintSubMem(Code *c,int num) = 0;
    virtual void PrintFuncHeaderName(Code *c,int num,char *name) = 0;
    virtual void PrintFuncHeaderAddr(Code *c,int num) = 0;
    virtual void PrintFuncFooter(Code *c,int num) = 0;
    virtual void PrintSubCodeSep(void) = 0;

    virtual char *Translate (csh handle,char *s, cs_insn *insn,int ends) = 0;

    Emit *Parse(char *str);
};

#endif // _LANGUAGE_H
