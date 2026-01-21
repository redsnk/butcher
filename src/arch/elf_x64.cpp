#include "elf_x64.hpp"
#include <capstone/capstone.h>

int Elf_x64::CheckFile(char *file_name) {
    OpenFile(file_name);
    if (elf != NULL) {
        CloseFile();
        return (true);
    }
    return (false);
}

int Elf_x64::OpenFile(char *file_name) {
    elf = GetELF(file_name);
    return(elf != NULL);
}

void Elf_x64::CloseFile(void) {
    FreeELF(elf);
}

uint8_t *Elf_x64::GetMemory(uint64_t addr,uint64_t size, uint64_t *read) {
    return(GetMemoryELF(elf,addr,size,read));
}

int Elf_x64::IsImportFunction (uint64_t addr, char *lib, char *func) {
struct _elf_import_name in;
        
    if (GetImportFunctionELF (elf,addr,&in)) {
        strcpy (lib,in.lib_name);
        strcpy (func,in.func_name);
        return (true);
    }
    return (false);
}
