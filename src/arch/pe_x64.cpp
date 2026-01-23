#include "pe_x64.hpp"
#include <capstone/capstone.h>

int Pe_x64::CheckFile(char *file_name) {
    OpenFile(file_name);
    if (pe != NULL) {
        CloseFile();
        return (true);
    }
    return (false);
}

int Pe_x64::OpenFile(char *file_name) {
    pe = GetPE(file_name);
    return(pe != NULL);
}

void Pe_x64::CloseFile(void) {
    FreePE(pe);
}

uint8_t *Pe_x64::GetMemory(uint64_t addr,uint64_t size, uint64_t *read) {
    return(GetMemoryPE(pe,addr,size,read));
}

int Pe_x64::IsImportFunction (uint64_t addr, char *lib, char *func) {
struct _import_name in;
        
    if (GetImportFunctionPE (pe,addr,&in)) {
        strcpy (lib,in.lib_name);
        strcpy (func,in.func_name);
        return (true);
    }
    return (false);
}

int Pe_x64::IsSymbolFunction (uint64_t addr, char *func) {
    return (false);
}

int Pe_x64::IsSymbolObject (uint64_t addr, char *name) {
    return (false);
}
