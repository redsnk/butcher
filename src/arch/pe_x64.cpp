#include "pe_x64.hpp"
#include "../files/pe.hpp"
#include <capstone/capstone.h>

int Pe_x64::OpenFile(char *file_name) {
    pe = GetPE(file_name);
    return(pe != NULL);
}

void Pe_x64::CloseFile(void) {
    FreePE(pe);
}

cs_err Pe_x64::Cs_open(void) {
    return(cs_open(CS_ARCH_X86, CS_MODE_64, &handle));
}

uint8_t *Pe_x64::GetMemory(uint64_t addr,uint64_t size) {
    return(GetMemoryPE(pe,addr,size));
}

