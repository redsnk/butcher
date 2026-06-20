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


#define ERR_CODE_OK         0

#define MAX_STR_OP     (1024*5)
#define MAX_PRINTF     (1024*1024)
#define BUFFER_STEP    (1024*100)

class Language {
    public:
    int lstdout;
    int len;
    int max_len;
    char *out;

    virtual const char *COMM(void) = 0;
    virtual int COMM_SEP(void) = 0;
    virtual const char *INDENT(void) = 0;
    virtual const char *ENDS(void) = 0;

    virtual const char *EQUAL(void) = 0;
    virtual const char *NEQUAL(void) = 0;
    virtual const char *GT(void) = 0;
    virtual const char *LT(void) = 0;
    virtual const char *GTE(void) = 0;
    virtual const char *LTE(void) = 0;
    virtual const char *ENDF(void) = 0;
    virtual const char *AND(void) = 0;
    virtual const char *OR(void) = 0;
    virtual const char *XOR(void) = 0;
    virtual const char *SHL(void) = 0;
    virtual const char *SHR(void) = 0;
    virtual const char *F_TRUE(void) = 0;
    virtual const char *F_FALSE(void) = 0;
    virtual const char *F_PUSH(void) = 0;
    virtual const char *F_POP(void) = 0;
    //virtual const char *F_POW(void) = 0;
    virtual const char *E_IFTHENELSE(void) = 0;
    virtual const char *E_IFTHEN(void) = 0;
    virtual const char *F_NOT(void) = 0;
    virtual const char *F_NEG(void) = 0;
    virtual const char *F_PSHUFD(void) = 0;
    virtual const char *E_NOT(void) = 0;
    virtual const char *E_LABEL_NAME(void) = 0;
    virtual const char *E_LABEL_ADDR(void) = 0;
    virtual const char *E_WHILE(void) = 0;
    virtual const char *E_BREAK(void) = 0;
    
    virtual const char *E_LABEL(void) = 0;
    virtual const char *E_LABEL_ANON(void) = 0;
    virtual const char *E_CALL_FROM_IAT(void) = 0;
    virtual const char *E_FUNC_NAME(void) = 0;
    virtual const char *E_FUNC_ADDR(void) = 0;
    //virtual const char *E_RETURN(void) = 0;
    virtual const char *E_GOTO(void) = 0;
    virtual const char *E_JMP_FROM_IAT(void) = 0;
    virtual const char *E_ENDIF(void) = 0;
    virtual const char *E_JCC_GOTO(void) = 0;
    virtual const char *E_SPACE(void) = 0;
    virtual const char *E_ANONC(void) = 0;
    virtual const char *F_MASK(void) = 0;

    virtual const char *E_ELSE(void) = 0;
    virtual const char *E_PANIC_JMP_INDEX(void) = 0;

    virtual void PrintHeader(Code *c) = 0;
    virtual void PrintAnonCalls(Code *c) = 0;
    virtual void PrintMainOpen(Code *c) = 0;
    virtual void PrintLoadError(const char *code,int num,const char *msg) = 0;
    virtual void PrintMainClose(Code *c,char *name) = 0;
    virtual void PrintSubMem(Code *c,int num) = 0;
    virtual void PrintFuncHeaderName(Code *c,int num,char *name) = 0;
    virtual void PrintFuncHeaderAddr(Code *c,int num) = 0;
    virtual void PrintFuncFooter(Code *c,int num) = 0;
    virtual void PrintSubCodeSep(void) = 0;

    virtual char *Translate (csh handle,const char *s, cs_insn *insn,int ends) = 0;

    void PrintF(const char *format,...);
    char *Indent(char *str);
    Emit *Parse(char *str);
};

#endif // _LANGUAGE_H
