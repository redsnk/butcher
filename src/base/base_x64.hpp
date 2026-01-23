#ifndef BASE_X64_H
#define BASE_X64_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include "../butcher.hpp"

class Base_x64 : public Butcher {
    public:
        cs_err Cs_open(void);
        int IsRet(cs_insn insn);
        int IsCall(cs_insn insn, uint64_t *addr, char **name);
        int IsJmp(cs_insn insn, uint64_t *addr);
        int IsJcc(cs_insn insn, uint64_t *addr);
        int IsInt(cs_insn insn, uint64_t *num);
        int IsEnd(cs_insn *insn, int n, int count);

        void PrintLine(cs_insn *insn,const char *format,...);
        int PrintInst(Code *c,struct _subcode *sc,int num);
        int PrintExtra(Code *c,struct _subcode *sc,int num);
        void PrintCodeC(Code *c);
        void PrintSubCodeC(Code *c,int num);
        void PrintSubMemC(Code *c,int num);
};

#endif // BASE_X64_H