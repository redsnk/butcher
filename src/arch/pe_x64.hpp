#ifndef PE_X64_H
#define PE_X64_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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

        uint8_t *PrintInst(cs_insn *insn);
        uint8_t *PrintCodeC(Code *c);
        uint8_t *PrintSubCodeC(Code *c,int num);
};

#endif // PE_X64_H