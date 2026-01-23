#ifndef PE_X64_H
#define PE_X64_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include "../base/base_x64.hpp"
#include "../files/pe.hpp"

class Pe_x64 : public Base_x64 {
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

#endif // PE_X64_H