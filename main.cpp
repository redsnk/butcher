#include <stdio.h>
#include <unistd.h>
#include "src/arch/arch_elf.hpp"
#include "src/arch/arch_pe.hpp"
#include "src/lang/lang_c.hpp"
#include "src/lang/lang_py.hpp"
#include "src/base/base_x64.hpp"

#define MY_VERSION  "v0.03"

#define MAX_STR     (1024)

int opt_t = false;
int opt_a = false;
enum Languages opt_l = C;

uint64_t string_to_num(char *num) {
uint64_t n;

    if ((strlen(num) >= 2) && (num[0] == '0') && (num[1] == 'x')) {
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
    switch (opt_l) {
        case PYTHON:
            l = new Lang_Py();
            break;
        case C:
        default:
            l = new Lang_C();
            break;
    }
    //l = new Lang_C();
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

const char* HELP = "\
--------------------------------------------------------------\n\
Butcher ("MY_VERSION") programed by Alex Bassas.\n\
--------------------------------------------------------------\n\
usage: Butcher [-l<lang>][-a][-t] <path> <addr>\n\
\n\
-l<lang>    => Output language: [c|p]\n\
                    - c -> C\n\
                    - p -> Python\n\
\n\
-t          => Enable traces\n\
-a          => Enable asm code\n\
\n";

int main (int argc, char **argv) {
int p,i;
char path[MAX_STR];
uint64_t addr;

    while ((p = getopt(argc,argv,"tal:")) != -1) {
        switch (p) {
                case 't':
                    opt_t = true;
                    break;
                case 'a':
                    opt_a = true;
                    break;
                case 'l':
                    switch (optarg[0]) {
                        case 'c':
                            opt_l = C;
                            break;
                        case 'p':
                            opt_l = PYTHON;
                            break;
                    }
                    break;
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
