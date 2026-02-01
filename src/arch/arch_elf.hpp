#ifndef ARCH_ELF_H
#define ARCH_ELF_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include "archive.hpp"
#include "../files/elf.hpp"

class Arch_Elf: public Archive {
    public:
        struct _ELF *elf;

        int CheckFile(char *file_name);
        int OpenFile(char *file_name);
        void CloseFile(void);
        int Is32(void);
        uint8_t *GetMemory(uint64_t addr,uint64_t size, uint64_t *read);
        int IsImportFunction (uint64_t addr, char **lib, char **func);
        int IsSymbolFunction (uint64_t addr, char **func);
        int IsSymbolObject (uint64_t addr, char **name);
};

#endif // ARCH_ELF_H