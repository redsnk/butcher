#include "arch_pe.hpp"
//#include <capstone/capstone.h>

int Arch_Pe::CheckFile(char *file_name) {
    OpenFile(file_name);
    if (pe != NULL) {
        CloseFile();
        return (true);
    }
    return (false);
}

int Arch_Pe::OpenFile(char *file_name) {
    pe = GetPE(file_name);
    return(pe != NULL);
}

void Arch_Pe::CloseFile(void) {
    FreePE(pe);
}

uint8_t *Arch_Pe::GetMemory(uint64_t addr,uint64_t size, uint64_t *read) {
    return(GetMemoryPE(pe,addr,size,read));
}

int Arch_Pe::IsImportFunction (uint64_t addr, char *lib, char *func) {
struct _import_name in;
        
    if (GetImportFunctionPE (pe,addr,&in)) {
        strcpy (lib,in.lib_name);
        strcpy (func,in.func_name);
        return (true);
    }
    return (false);
}

int Arch_Pe::IsSymbolFunction (uint64_t addr, char *func) {
    return (false);
}

int Arch_Pe::IsSymbolObject (uint64_t addr, char *name) {
    return (false);
}
