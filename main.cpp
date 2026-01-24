#include <stdio.h>
#include <unistd.h>
//#include "src/arch/pe_x64.hpp"
//#include "src/arch/elf_x64.hpp"
#include "src/arch/arch_elf.hpp"
#include "src/arch/arch_pe.hpp"
#include "src/lang/lang_c.hpp"
#include "src/base/base_x64.hpp"

#define MY_VERSION  "v0.02"

#define MAX_STR     (1024)

#define HELP "\
--------------------------------------------------------------\n\
Butcher ("MY_VERSION") programed by Alex Bassas.\n\
--------------------------------------------------------------\n\
usage: Butcher <path> <addr>\n\
\n"

uint64_t string_to_num(char *num) {
uint64_t n;

    if ((strlen(num) >= 2) && (num[0] == '0') && (num[1] == 'x')) {
        // 0x1BBA
        sscanf(num+2,"%llx",&n);
        return (n);
    }
    return (atol(num));
}

int butcher(char *path,uint64_t addr) {
Butcher *b;
Language *l;
Archive *a;


    /*
    Butcher *b = new Pe_x64();
    if (!b->CheckFile(path)) {
        delete b;
        b = new Elf_x64();
        if (!b->CheckFile(path)) {
            delete b;
            printf("Error: format not compatible.\n");
            return (0);
        }
    }
    */
    l = new Lang_C();
    a = new Arch_Pe();
    if (!a->CheckFile(path)) {
        delete a;
        a = new Arch_Elf();
        if (!a->CheckFile(path)) {
            delete a;
            delete l;
            printf("Error: format not compatible.\n");
            return (0);
        }
    }
    b = new Base_x64(a,l);
    b->Cut(path,addr);
    delete b;
    return (1);
}

int main (int argc, char **argv) {
int p,i;
char path[MAX_STR];
uint64_t addr;

    while ((p = getopt(argc,argv,"")) != -1) {
        switch (p) {
                case '?':
                default:
                        printf(HELP);
                        exit(0);
        }
    }
    path[0] = 0;
    if (optind == argc) {
        printf(HELP);
        exit(0);
    }
    if ((optind+1)<argc) {
        strcpy(path,argv[optind]);
        addr = string_to_num(argv[optind+1]);
    }
    else {
        printf(HELP);
        exit(0);
    }
    return(butcher(path,addr));
}
