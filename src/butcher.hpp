#ifndef _BUTCHER_H
#define _BUTCHER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <list>
#include <set>
#include <algorithm>
#include <cstring>
#include <capstone/capstone.h>
#include "code.hpp"
#include "arch/archive.hpp"
#include "lang/language.hpp"

class Butcher {
    public:
        csh handle;
        Archive *arch;
        Language *lang;

        virtual cs_err Cs_open(void) = 0;
        virtual int IsRet(cs_insn insn) = 0;
        virtual int IsCall(cs_insn insn, uint64_t *addr) = 0;
        virtual int IsJmp(cs_insn insn, uint64_t *addr) = 0;
        virtual int IsJcc(cs_insn insn, uint64_t *addr) = 0;
        virtual int IsInt(cs_insn insn, uint64_t *num) = 0;
        virtual int IsEnd(cs_insn *insn, int n, int count) = 0;
        virtual void PrintCode(Code *c) = 0;
        //
        Butcher(Archive *a,Language *l);
        ~Butcher();
        int IsGroup (cs_insn insn, int group);
        Code *GetCode(Code *c,uint64_t address,char *name,int parent);
        void Cut(char *file_name,uint64_t address);
};

#endif // _BUTCHER_H