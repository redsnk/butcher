// https://gist.github.com/x0nu11byt3/bcb35c3de461e5fb66173071a2379779

#include "elf.hpp"

struct _ELF *GetELF (char *name) {
    return (NULL);
}

void FreeELF (struct _ELF *elf) {

}

uint8_t *GetMemoryELF (struct _ELF *elf, uint64_t addr, uint64_t size, uint64_t *read) {
    return (NULL);
}

int GetImportFunction (struct _ELF *elf, uint64_t addr, struct _elf_import_name *in) {
    return (false);
}

