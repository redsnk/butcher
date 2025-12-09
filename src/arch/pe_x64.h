#ifndef PE_X64_H
#define PE_X64_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

char *Cut_pe_x64(char *file_name,uint64_t address,char *func_name);

#endif // PE_X64_H