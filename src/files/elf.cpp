// https://gist.github.com/x0nu11byt3/bcb35c3de461e5fb66173071a2379779
// https://web.mit.edu/freebsd/head/sys/sys/elf32.h
// https://web.mit.edu/freebsd/head/sys/sys/elf64.h

#include "elf.hpp"

struct _ELF *GetELF (char *name) {
FILE *f;
struct _ELF *elf;
long l,s;

	f = fopen(name,"r");
	if (f!= NULL) {
		elf = (struct _ELF *) malloc(sizeof(struct _ELF));
		if (elf != NULL) {
			// FILE
			elf->f = f;
			// Executable Header
			l = fread(&elf->Ehdr,1,sizeof(union _Elf_Ehdr),f);
			if (l == sizeof(union _Elf_Ehdr)) {
                // "\x7fELF"
                if (*((uint32_t *)(elf->Ehdr.Ehdr64.e_ident)) == 0x464C457F) {
                    if (elf->Ehdr.Ehdr64.e_machine == EM_X86_64) {
                        // Program Headers
                        s = elf->Ehdr.Ehdr64.e_phnum*elf->Ehdr.Ehdr64.e_phentsize;
                        elf->Phdr = (Elf64_Phdr *) malloc(s);
                        if (elf->Phdr != NULL) {
                            if (fseek(f,elf->Ehdr.Ehdr64.e_phoff,SEEK_SET) != -1) {
                                l = fread(elf->Phdr,1,s,f);
                                if (l == s) {
                                    // Section Headers
                                    s = elf->Ehdr.Ehdr64.e_shnum*elf->Ehdr.Ehdr64.e_shentsize;
                                    elf->Shdr = (Elf64_Shdr *) malloc(s);
                                    if (elf->Shdr != NULL) {
                                        if (fseek(f,elf->Ehdr.Ehdr64.e_shoff,SEEK_SET) != -1) {
                                            l = fread(elf->Phdr,1,s,f);
                                            if (l == s) {
                                                return (elf);
                                            }
                                            else {
                                                printf("GetELF error: fread Shdr\n");
                                            }
                                        }
                                        else {
                                            printf("GetELF error: fseek Shdr\n");
                                        }
                                        free(elf->Shdr);
                                    }
                                    else {
                                        printf("GetELF error: Shdr malloc\n");
                                    }
                                }
                                else {
                                    printf("GetELF error: fread Phdr\n");
                                }
                            }
                            else {
                                printf("GetELF error: fseek Phdr\n");
                            }
                            free(elf->Phdr);
                        }
                        else {
                            printf("GetELF error: Phdr malloc\n");
                        }
                    }
                    else {
                        printf("GetELF error: Architecture not compatible\n");
                    }
                }
                else {
                    printf("GetELF error: read signature\n");
                }
            }
            free (elf);
        }
        fclose(f);
    }
    return (NULL);
}

void FreeELF (struct _ELF *elf) {
    if (elf != NULL) {
        free(elf->Phdr);
        free(elf->Shdr);
		fclose(elf->f);
		free(elf);
	}
}

uint8_t *GetMemoryELF (struct _ELF *elf, uint64_t addr, uint64_t size, uint64_t *read) {
int n;
uint64_t start, end, offset;
uint8_t *m;
long l;

    for (n=0;n<elf->Ehdr.Ehdr64.e_phnum;n++) {
        start = elf->Phdr[n].p_vaddr;
        end = elf->Phdr[n].p_vaddr + elf->Phdr[n].p_memsz - 1; 
        if ((addr >= start) && (addr < end)) {
			// addr inside the section
			if ((end-addr) < size) {
				// not enought size
				size = end-addr-1;
				if (!size) {
					// size == 0
					return (NULL);
				}
			}
			m = (uint8_t *) malloc(size);
			if (m != NULL) {
				offset = elf->Phdr[n].p_offset+(addr-start);
				l = fseek(elf->f,offset,SEEK_SET);
				if (l != -1) {
					l = fread(m,1,size,elf->f);
					if (l == size) {
						*read = size;
						return (m);
					} else {
						printf("GetMemoryELF error: fread\n");
					}
				} else {
					printf("GetMemoryELF error: fseek\n");
				}
				free(m);		
			}
            else {
                printf("GetMemoryELF error: malloc\n");
            }
			break;
		}
    }
    return (NULL);
}

int GetImportFunctionELF (struct _ELF *elf, uint64_t addr, struct _elf_import_name *in) {
    return (false);
}

