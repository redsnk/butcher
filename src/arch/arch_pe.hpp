#ifndef ARCH_PE_H
#define ARCH_PE_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include "archive.hpp"
#include "../files/pe.hpp"

class Arch_Pe : public Archive {
    public:
        struct _PE *pe;

        int CheckFile(char *file_name);
        int OpenFile(char *file_name);
        void CloseFile(void);
        uint8_t *GetMemory(uint64_t addr,uint64_t size, uint64_t *read);
        int IsImportFunction (uint64_t addr, char *lib, char *func);
        int IsSymbolFunction (uint64_t addr, char *func);
        int IsSymbolObject (uint64_t addr, char *name);
};

#endif // ARCH_PE_H