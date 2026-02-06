#include "arch_pe.hpp"
//#include <capstone/capstone.h>

int Arch_Pe::CheckFile(char *file_name) {
    OpenFile(file_name);
    if (pe != NULL) {
        CloseFile();
        return (true);
    }
    return (false);
}

int Arch_Pe::OpenFile(char *file_name) {
    pe = GetPE(file_name);
    return(pe != NULL);
}

char *Arch_Pe::GetFileName(void) {
    return (pe->name);
}

void Arch_Pe::CloseFile(void) {
    FreePE(pe);
}

int Arch_Pe::Is32(void) {
    return (pe->pe32);
}

uint8_t *Arch_Pe::GetMemory(uint64_t addr,uint64_t size, uint64_t *read) {
    return(GetMemoryPE(pe,addr,size,read));
}

int Arch_Pe::ValidMemory(uint64_t addr) {
 uint8_t *mem;
 uint64_t read;

    mem = GetMemory(addr,1,&read);
    if (mem != NULL) {
        free(mem);
        return (true);
    }
    return (false);
}

int Arch_Pe::IsImportFunction (uint64_t addr, char **lib, char **func) {
    if (GetImportFunctionPE (pe,addr,lib,func)) {
        /*
        *lib = strdup(in.lib_name);
        *func = strdup(in.func_name);
        */
        return (true);
    }
    return (false);
}

int Arch_Pe::IsSymbolFunction (uint64_t addr, char **func) {
    return (false);
}

int Arch_Pe::IsSymbolObject (uint64_t addr, char **name) {
    return (false);
}

struct _Section *Arch_Pe::GetSections(int *count) {
int n;
uint64_t base;
struct _Section *sections;

    base = GetImageBase(pe);
    sections = (struct _Section *) malloc(sizeof(struct _Section)*pe->COFF_File_Header.NumberOfSections);
    *count = pe->COFF_File_Header.NumberOfSections;
	for (n=0;n<pe->COFF_File_Header.NumberOfSections;n++) {
        sections[n].d_Offset = pe->Sections[n].PointerToRawData;
        sections[n].d_Size = pe->Sections[n].SizeOfRawData;
        sections[n].v_Address = base+pe->Sections[n].VirtualAddress;
        sections[n].v_Size = pe->Sections[n].VirtualSize;
	}
    return (sections);
}
