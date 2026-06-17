// https://gist.github.com/x0nu11byt3/bcb35c3de461e5fb66173071a2379779
// https://web.mit.edu/freebsd/head/sys/sys/elf32.h
// https://web.mit.edu/freebsd/head/sys/sys/elf64.h
// readelf

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

/*
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
*/

int GetSectionIndexByType (struct _ELF *elf,Elf32_Word sh_type) {
int n;
uint8_t *mem;
long l;

    for (n=0;n<elf->Ehdr.Ehdr64.e_shnum;n++) {
        if (elf->Shdr[n].sh_type == sh_type) {
            return (n);
        }
    }
    return (-1);
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

int GetElfSections (struct _ELF *elf) {
int i;
/*
    // Section Headers String Table (to locate sectio names)
    elf->ShStrTable = (char *) GetSectionByIndex(elf,elf->Ehdr.Ehdr64.e_shstrndx,&elf->ShStrTable_size);
    if (elf->ShStrTable != NULL) {
        // Dynamic linker symbol table ".dynsym"
        elf->DynSymTable = (Elf64_Sym *) GetSectionByType(elf,SHT_DYNSYM,&elf->DynSymTable_size);
        if (elf->DynSymTable != NULL) {
            elf->DynSymTable_count = elf->DynSymTable_size/sizeof(Elf64_Sym);
            // Dynamic strings table ".dynstr"
            elf->DynStrTable = (char *) GetSectionByName(elf,".dynstr",&elf->DynStrTable_size);
            if (elf->DynStrTable != NULL) {
                // Got.Plt Table ".got.plt"
                elf->GotPltTableIndex = GetSectionIndexByName(elf,".got.plt");
                if (elf->GotPltTableIndex != -1) {
                    // symbol table ".symtab"
                    elf->SymTable = (Elf64_Sym *) GetSectionByName(elf,".symtab",&elf->SymTable_size);
                    if (elf->SymTable != NULL) {
                        elf->SymTable_count = elf->SymTable_size/sizeof(Elf64_Sym);
                    }
                    // strings symbole table ".strtab"
                    elf->StrTable = (char *) GetSectionByName(elf,".strtab",&elf->StrTable_size);
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
*/

    // Section Headers String Table (to locate sectio names)
    elf->ShStrTable = (char *) GetSectionByIndex(elf,elf->Ehdr.Ehdr64.e_shstrndx,&elf->ShStrTable_size);
    if (elf->ShStrTable != NULL) {
        // Dynamic linker symbol table ".dynsym"
        //elf->DynSymTable = (Elf64_Sym *) GetSectionByType(elf,SHT_DYNSYM,&elf->DynSymTable_size);
        i = GetSectionIndexByType(elf,SHT_DYNSYM);
        if (i > 0) {
            elf->DynSymTable = (Elf64_Sym *) GetSectionByIndex (elf,i,&elf->DynSymTable_size);
            elf->DynSymTable_count = elf->DynSymTable_size/sizeof(Elf64_Sym);
            // Dynamic strings table ".dynstr"
            elf->DynStrTable = (char *) GetSectionByName(elf,".dynstr",&elf->DynStrTable_size);
            if (elf->DynStrTable != NULL) {
                // Got.Plt Table ".got.plt"
                elf->GotPltTableIndex = GetSectionIndexByName(elf,".got.plt");
                if (elf->GotPltTableIndex != -1) {
                    // symbol table ".symtab"
                    elf->SymTable = (Elf64_Sym *) GetSectionByName(elf,".symtab",&elf->SymTable_size);
                    if (elf->SymTable != NULL) {
                        elf->SymTable_count = elf->SymTable_size/sizeof(Elf64_Sym);
                    }
                    // strings symbole table ".strtab"
                    elf->StrTable = (char *) GetSectionByName(elf,".strtab",&elf->StrTable_size);
                    // Minimum versions of symbols required from external shared libraries ".gnu.version_r"
                    i = GetSectionIndexByType(elf,SHT_GNU_verneed);
                    if (i > 0) {
                        elf->VerNeed = (char *) GetSectionByIndex (elf,i,&elf->VerNeed_size);
                        // sh_info holds the number of Verneed entries
                        elf->VerNeed_count = elf->Shdr[i].sh_info;
                        // '.gnu.version'
                        i = GetSectionIndexByType(elf,SHT_GNU_versym);
                        if (i > 0) {
                            elf->VerSym = (uint16_t *) GetSectionByIndex (elf,i,&elf->VerSym_size);
                            elf->VerSym_count = elf->VerSym_size / sizeof(uint16_t);
                            return (true);
                        }
                        free(elf->VerNeed);
                    }
                    else {
                        printf("GetELF error: '.gnu.version_r'\n");
                    }
                }
                else {
                    printf("GetELF error: '.got.plt'\n");
                }
                free(elf->DynStrTable);
            }
            free(elf->DynSymTable);
        }
        free(elf->ShStrTable);
    }
    else {
        printf("GetELF error: '.dynsym'\n");
    }
    return (false);
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
                                                if (GetElfSections(elf)) {
                                                    elf->name = strdup(name);
                                                    return (elf);
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
        free(elf->VerSym);
        free(elf->VerNeed);
        if (elf->StrTable != NULL) free(elf->StrTable);
        if (elf->SymTable != NULL) free(elf->SymTable);
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

    // The program header table provides a segment view of the binary
    for (n=0;n<elf->Ehdr.Ehdr64.e_phnum;n++) {
        m = GetMemoryFile(elf->f,addr,size,elf->Phdr[n].p_vaddr,elf->Phdr[n].p_memsz,elf->Phdr[n].p_offset,elf->Phdr[n].p_filesz,read);
        if (m != NULL) {
            return (m);
        }
    }
    return (NULL);
}

void GetLibraryElf (struct _ELF *elf,int shndx,int index,char **lib) {
int ver_idx,n,m,vna_other;
char *pos,*aux_pos;
Elf64_Verneed *ned;
Elf64_Vernaux *aux;

    if (shndx != 0) {
        *lib = strdup("self");
    }
    else if (index < elf->VerSym_count) {
        // strip HIDDEN bit
        ver_idx =  elf->VerSym[index] & 0x7FFF;
        pos = elf->VerNeed;
        for (n=0;n<elf->VerNeed_count;n++) {
            ned = (Elf64_Verneed *) pos;
            aux_pos = pos+ned->vn_aux;
            for(m=0;m<ned->vn_cnt;m++) {
                aux = ((Elf64_Vernaux *) aux_pos);
                vna_other = aux->vna_other;
                if (vna_other == ver_idx) {
                    *lib = strdup(elf->DynStrTable+ned->vn_file);
                    return;
                }
                if (aux->vna_next == 0) {
                    break;
                }
                aux_pos += aux->vna_next;
            }
            pos += ned->vn_next;
        }
    }
    *lib = strdup("undefined");
}

int GetImportFunctionELF (struct _ELF *elf, uint64_t addr, char **lib, char **func) {
uint64_t start,end,index;

    start = elf->Shdr[elf->GotPltTableIndex].sh_addr;
    end = start + elf->Shdr[elf->GotPltTableIndex].sh_size - 1;
    if ((addr >= start) && (addr <= end)) {
        // TODO; Elf32
        index = ((addr-start) / 8)+1;
        //strcpy (in->lib_name,"<none>");
        //*lib = strdup("undefined");
        GetLibraryElf(elf,elf->DynSymTable[index].st_shndx,index,lib);
        //strcpy (in->func_name,elf->DynStrTable+elf->DynSymTable[index].st_name);
        *func = strdup(elf->DynStrTable+elf->DynSymTable[index].st_name);
        return (true);
    }
    return (false);
}

int GetSymbolELF (struct _ELF *elf,uint64_t addr,char **name,unsigned char *info) {
int n;
char *p;

    for (n=0;n<elf->DynSymTable_count;n++) {
        if (elf->DynSymTable[n].st_value == addr) {
            p = elf->DynStrTable+elf->DynSymTable[n].st_name;
            *name = (char *) malloc(strlen(p)+16);
            strcpy(*name,"dyn_");
            strcat(*name,p);
            //*name = strdup(elf->DynStrTable+elf->DynSymTable[n].st_name);
            *info = elf->DynSymTable[n].st_info;
            return (true);
        }
    }
    if (elf->SymTable != NULL) {
        for (n=0;n<elf->SymTable_count;n++) {
            if (elf->SymTable[n].st_value == addr) {
                //strcpy(name,elf->DynStrTable+elf->DynSymTable[n].st_name);
                p = elf->StrTable+elf->SymTable[n].st_name;
                //*name = strdup(elf->StrTable+elf->SymTable[n].st_name);
                *name = (char *) malloc(strlen(p)+16);
                strcpy(*name,"sym_");
                strcat(*name,p);
                *info = elf->DynSymTable[n].st_info;
                return (true);
            }
        }
    }
    return (false);
}

