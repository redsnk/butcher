#include "arch_elf.hpp"
//#include <capstone/capstone.h>

int Arch_Elf::CheckFile(char *file_name) {
    OpenFile(file_name);
    if (elf != NULL) {
        CloseFile();
        return (true);
    }
    return (false);
}

int Arch_Elf::OpenFile(char *file_name) {
    elf = GetELF(file_name);
    return(elf != NULL);
}

void Arch_Elf::CloseFile(void) {
    FreeELF(elf);
}

uint8_t *Arch_Elf::GetMemory(uint64_t addr,uint64_t size, uint64_t *read) {
    return(GetMemoryELF(elf,addr,size,read));
}

int Arch_Elf::IsImportFunction (uint64_t addr, char **lib, char **func) {
struct _elf_import_name in;
        
    if (GetImportFunctionELF (elf,addr,&in)) {
        *lib = strdup(in.lib_name);
        *func = strdup(in.func_name);
        return (true);
    }
    return (false);
}

int Arch_Elf::IsSymbolFunction (uint64_t addr, char **func) {
unsigned char info;

    if (GetSymbolELF(elf,addr,func,&info)) {
        return  (ELF64_ST_TYPE(info) == STT_FUNC);
    }
    return (false);
}

int Arch_Elf::IsSymbolObject (uint64_t addr, char **name) {
unsigned char info;

    if (GetSymbolELF(elf,addr,name,&info)) {
        return  (ELF64_ST_TYPE(info) == STT_OBJECT);
    }
    return (false);
}
