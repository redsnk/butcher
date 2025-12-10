#ifndef _BUTCHER_H
#define _BUTCHER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <list>
#include <capstone/capstone.h>

struct _subcode {
    uint64_t first;
    uint64_t last;
    cs_insn *insn;
    size_t count;
};

class Code {
    public:
        struct _subcode *subcodes;
        int count;
        uint64_t ep;

        Code(uint64_t addr);
        ~Code();
        void AddSubcode (struct _subcode sc);
        int HasAddr (uint64_t addr);
        void Print (void);
};

class Butcher {
    public:
        csh handle;

        virtual int OpenFile(char *file_name) = 0;
        virtual cs_err Cs_open(void) = 0;
        virtual void CloseFile(void) = 0;
        virtual uint8_t *GetMemory(uint64_t addr,uint64_t size) = 0;
        virtual int IsRet(cs_insn insn) = 0;
        virtual int IsCall(cs_insn insn, uint64_t *addr) = 0;
        virtual int IsJmp(cs_insn insn, uint64_t *addr) = 0;
        virtual uint8_t *PrintCodeC(Code *c) = 0;
        //
        int IsGroup (cs_insn insn, int group);
        Code *GetCode(Code *c,uint64_t address, int max_subcode);
        uint8_t *Cut(char *file_name,uint64_t address,char *func_name);
};

#endif // _BUTCHER_H