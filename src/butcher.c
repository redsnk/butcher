

#include "butcher.h"
#include "arch/pe_x64.h"

char *Cut(char *file_name,uint64_t address,char *func_name) {
    // Only PE/X64
    return (Cut_pe_x64(file_name,address,func_name));
}