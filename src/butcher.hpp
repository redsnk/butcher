#ifndef _BUTCHER_H
#define _BUTCHER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <cstring>
#include <capstone/capstone.h>
#include "code.hpp"
#include "arch/archive.hpp"
#include "lang/language.hpp"

#define UNDEF_ADDR_JMP    0

class Butcher {
    public:
        csh handle;
        Archive *arch;
        Language *lang;
        int ltraces;
        int lasm;
        int loadm;
        std::set<uint64_t> ex;
        std::map<uint64_t, std::string> named;

        virtual cs_err Cs_open(void) = 0;
        virtual int IsSubMem(cs_insn *insn, uint64_t *addr, uint8_t **mem, int *count) = 0;
        virtual int IsRet(cs_insn *insn) = 0;
        virtual int IsCall(cs_insn *insn, uint64_t *addr) = 0;
        virtual int IsJmp(cs_insn *insn, uint64_t *addr[],int *count) = 0;
        virtual int IsJcc(cs_insn *insn, uint64_t *addr) = 0;
        virtual int IsInt(cs_insn *insn, uint64_t *num) = 0;
        virtual int IsEnd(cs_insn *insn, int n, int count) = 0;
        //virtual int IsJmpIAT(cs_insn *insn) = 0;
        virtual void PrintCode(Code *c) = 0;
        //
        Butcher(Archive *a,Language *l);
        ~Butcher();
        int IsGroup (cs_insn *insn, int group);
        int Excluded(uint64_t addr);
        int IsNamedFunction (uint64_t addr, char **func);
        Code *GetCode(Code *c,uint64_t address,char *name,int parent);
        void Cut(char *file_name,uint64_t address);
};

#endif // _BUTCHER_H