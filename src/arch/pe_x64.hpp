#ifndef PE_X64_H
#define PE_X64_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include "../butcher.hpp"

class Pe_x64 : public Butcher {
    public:
        struct _PE *pe;

        int OpenFile(char *file_name);
        cs_err Cs_open(void);
        void CloseFile(void);
        uint8_t *GetMemory(uint64_t addr,uint64_t size);
        int IsRet(cs_insn insn);
        int IsCall(cs_insn insn, uint64_t *addr);
        int IsJmp(cs_insn insn, uint64_t *addr);
        int IsInt(cs_insn insn, uint64_t *num);
        int IsImport(cs_insn insn, char **name);

        int PrintInst(Code *c,struct _subcode *sc,int num);
        int PrintExtra(Code *c,struct _subcode *sc,int num);
        void PrintCodeC(Code *c);
        void PrintSubCodeC(Code *c,int num);
        void PrintSubMemC(Code *c,int num);
};

#endif // PE_X64_H