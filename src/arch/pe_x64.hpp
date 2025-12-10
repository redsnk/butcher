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
};

#endif // PE_X64_H