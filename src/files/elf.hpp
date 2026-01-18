#ifndef _ELF_H
#define _ELF_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>

struct _ELF {
	FILE *f;
	
};

#define MAX_IMPORT_NAME	(256)

struct _elf_import_name {
	char lib_name[MAX_IMPORT_NAME];
	char func_name[MAX_IMPORT_NAME];
};


struct _ELF *GetELF (char *name);
void FreeELF (struct _ELF *elf);
uint8_t *GetMemoryELF (struct _ELF *elf, uint64_t addr, uint64_t size, uint64_t *read);
int GetImportFunction (struct _ELF *elf, uint64_t addr, struct _elf_import_name *in);


#endif	// _ELF_H
