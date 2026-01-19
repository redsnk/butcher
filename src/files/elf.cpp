// https://gist.github.com/x0nu11byt3/bcb35c3de461e5fb66173071a2379779
// https://web.mit.edu/freebsd/head/sys/sys/elf32.h
// https://web.mit.edu/freebsd/head/sys/sys/elf64.h

#include "elf.hpp"

struct _ELF *GetELF (char *name) {
FILE *f;
struct _ELF *elf;
int l;

	f = fopen(name,"r");
	if (f!= NULL) {
		elf = (struct _ELF *) malloc(sizeof(struct _ELF));
		if (elf != NULL) {
			// FILE
			elf->f = f;
			// Executable Header
			l = fread(&elf->Ehdr,1,sizeof(union _Elf_Ehdr),f);
			if (l == sizeof(union _Elf_Ehdr)) {

            }
            free (elf);
        }
        fclose(f);
    }
    return (NULL);
}

void FreeELF (struct _ELF *elf) {
    if (elf != NULL) {
		fclose(elf->f);
		free(elf);
	}
}

uint8_t *GetMemoryELF (struct _ELF *elf, uint64_t addr, uint64_t size, uint64_t *read) {
    return (NULL);
}

int GetImportFunction (struct _ELF *elf, uint64_t addr, struct _elf_import_name *in) {
    return (false);
}

