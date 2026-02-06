// https://gist.github.com/x0nu11byt3/bcb35c3de461e5fb66173071a2379779
// https://web.mit.edu/freebsd/head/sys/sys/elf32.h
// https://web.mit.edu/freebsd/head/sys/sys/elf64.h

#include "elf.hpp"

uint8_t *GetSectionByIndex (struct _ELF *elf,int index,uint64_t *size) {
uint8_t *mem;
long l;

    mem = (uint8_t *) malloc(elf->Shdr[index].sh_size);
    if (mem != NULL) {
        if (fseek(elf->f,elf->Shdr[index].sh_offset,SEEK_SET) != -1) {
            *size = elf->Shdr[index].sh_size;
            l = fread(mem,1,*size,elf->f);
            if (l == *size) {
                return (mem);
            }
        }
        free (mem);
    }
    return (NULL);
}

uint8_t *GetSectionByType (struct _ELF *elf,Elf32_Word sh_type,uint64_t *size) {
int n;
uint8_t *mem;
long l;

    for (n=0;n<elf->Ehdr.Ehdr64.e_shnum;n++) {
        if (elf->Shdr[n].sh_type == sh_type) {
            return (GetSectionByIndex(elf,n,size));
        }
    }
    return (NULL);
}

int GetSectionIndexByName (struct _ELF *elf,const char *name) {
int n;
char *sname;

    for (n=0;n<elf->Ehdr.Ehdr64.e_shnum;n++) {
        sname = (char *) (elf->ShStrTable + elf->Shdr[n].sh_name);
        if (!strcmp(name,sname)) {
            return (n);
        }
    }
    return (-1);
}

uint8_t *GetSectionByName (struct _ELF *elf,const char *name,uint64_t *size) {
int i;
uint8_t *mem;
long l;

    i = GetSectionIndexByName(elf,name);
    if (i != -1) {
        return (GetSectionByIndex(elf,i,size));
    }
    return (NULL);
}


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
                                            l = fread(elf->Shdr,1,s,f);
                                            if (l == s) {
                                                // Section Headers string table
                                                elf->ShStrTable = (char *) GetSectionByIndex(elf,elf->Ehdr.Ehdr64.e_shstrndx,&elf->ShStrTable_size);
                                                if (elf->ShStrTable != NULL) {
                                                    // Dynamic linker symbol table
                                                    elf->DynSymTable = (Elf64_Sym *) GetSectionByType(elf,SHT_DYNSYM,&elf->DynSymTable_size);
                                                    if (elf->DynSymTable != NULL) {
                                                        elf->DynSymTable_count = elf->DynSymTable_size/sizeof(Elf64_Sym);
                                                        // Dynamic strings table
                                                        elf->DynStrTable = (char *) GetSectionByName(elf,".dynstr",&elf->DynStrTable_size);
                                                        if (elf->DynStrTable != NULL) {
                                                            // Got.Plt Table
                                                            elf->GotPltTableIndex = GetSectionIndexByName(elf,".got.plt");
                                                            if (elf->GotPltTableIndex != -1) {
                                                                elf->name = strdup(name);
                                                                return (elf);
                                                            }
                                                            else {
                                                                printf("GetELF error: GetSectionIndexByName\n");
                                                            }
                                                            free(elf->DynStrTable);
                                                        }
                                                        free(elf->DynSymTable);
                                                    }
                                                    free(elf->ShStrTable);
                                                }
                                                else {
                                                    printf("GetELF error: GetSectionByIndex\n");
                                                }
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
        free(elf->name);
        free(elf->DynStrTable);
        free(elf->DynSymTable);
        free(elf->ShStrTable);
        free(elf->Phdr);
        free(elf->Shdr);
		fclose(elf->f);
		free(elf);
	}
}

uint8_t *GetMemoryELF (struct _ELF *elf, uint64_t addr, uint64_t size, uint64_t *read) {
int n;
uint8_t *m;

    for (n=0;n<elf->Ehdr.Ehdr64.e_phnum;n++) {
        m = GetMemoryFile(elf->f,addr,size,elf->Phdr[n].p_vaddr,elf->Phdr[n].p_memsz,elf->Phdr[n].p_offset,elf->Phdr[n].p_filesz,read);
        if (m != NULL) {
            return (m);
        }
    }
    return (NULL);
}

int GetImportFunctionELF (struct _ELF *elf, uint64_t addr, char **lib, char **func) {
uint64_t start,end,index;

    start = elf->Shdr[elf->GotPltTableIndex].sh_addr;
    end = start + elf->Shdr[elf->GotPltTableIndex].sh_size - 1;
    if ((addr >= start) && (addr <= end)) {
        // TODO; Elf32
        index = ((addr-start) / 8)+1;
        //strcpy (in->lib_name,"<none>");
        *lib = strdup("<none>");
        //strcpy (in->func_name,elf->DynStrTable+elf->DynSymTable[index].st_name);
        *func = strdup(elf->DynStrTable+elf->DynSymTable[index].st_name);
        return (true);
    }
    return (false);
}

int GetSymbolELF (struct _ELF *elf,uint64_t addr,char **name,unsigned char *info) {
int n;

    for (n=0;n<elf->DynSymTable_count;n++) {
        if (elf->DynSymTable[n].st_value == addr) {
            //strcpy(name,elf->DynStrTable+elf->DynSymTable[n].st_name);
            *name = strdup(elf->DynStrTable+elf->DynSymTable[n].st_name);
            *info = elf->DynSymTable[n].st_info;
            return (true);
        }
    }
    return (false);
}

