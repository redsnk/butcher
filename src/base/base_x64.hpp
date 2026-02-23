#ifndef BASE_X64_H
#define BASE_X64_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include "../butcher.hpp"
#include "../lang/lang_x64.hpp"

#define STACK_ADDR		0xF000
#define STACK_SIZE		(1024*10)

class Base_x64 : public Butcher {
    public:
        Lang_x64 *lang_x64;
        
        cs_err Cs_open(void);
        int IsSubMem(cs_insn *insn, uint64_t *addr, uint8_t **mem, int *count);
        int IsRet(cs_insn *insn);
        int IsCall(cs_insn *insn, uint64_t *addr);
        int IsJmp(cs_insn *insn, uint64_t *addr[],int *count);
        int IsJcc(cs_insn *insn, uint64_t *addr);
        int IsInt(cs_insn *insn, uint64_t *num);
        int IsEnd(cs_insn *insn, int n, int count);
        int IsJmpIAT(cs_insn *insn);

        //using Butcher::Butcher;
        void PrintLine(cs_insn *insn,int indent,const char *format,...);
        void PrintLabel(Code *c,uint64_t addr);
        int PrintInst(Code *c,struct _subcode *sc,int num);
        int PrintExtra(Code *c,struct _subcode *sc,int num);
        int IsFSGS(cs_insn *insn);
        void PrintCode(Code *c);
        void PrintSubCode(Code *c,int num);

        Base_x64(Archive *a,Language *l);
};

#endif // BASE_X64_H