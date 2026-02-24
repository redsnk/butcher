#include <stdio.h>
#include <unistd.h>
#include "src/arch/arch_elf.hpp"
#include "src/arch/arch_pe.hpp"
#include "src/lang/lang_c_x64.hpp"
#include "src/lang/lang_py_x64.hpp"
#include "src/base/base_x64.hpp"

#define MY_VERSION  "v0.01"

#define MAX_STR     (1024)

int opt_t = false;
int opt_a = false;
int opt_m = false;
enum Languages opt_l = C;
std::set<uint64_t> exclude;
std::map<uint64_t, std::string> named;

uint64_t string_to_num(char *num) {
uint64_t n;

    if ((strlen(num) >= 2) && (num[0] == '0') && (num[1] == 'x')) {
        sscanf(num+2,"%llx",&n);
        return (n);
    }
    return (atol(num));
}

void parse_ex(char *list) {
char *t;
char  *p,*i;

    t = strdup(list);
    i = p = t;
    while (*i) {
        p = strchr(i,',');
        if (p != NULL) {
            *p = 0;
            exclude.insert(string_to_num(i));
            i = p+1;
        }
        else {
            exclude.insert(string_to_num(i));
            break;
        }
    }
    free(t);
}

#define MAX_BUFFER  (1024)

void parse_named(char *name) {
FILE *f;
char buffer[MAX_BUFFER];
char *p;

    f = fopen(name,"r");
    if (f != NULL) {
        while (fgets(buffer,MAX_BUFFER-1,f) != NULL) {
            while ((p=strchr(buffer,'\n'))!= NULL) *p = 0;
            p = strchr(buffer,',');
            if (p != NULL) {
                *p = 0;
                named.insert({ string_to_num(buffer),p+1 });
            }
        }
        fclose(f);
    }
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
            l = (Language *) new Lang_Py_x64();
            break;
        case C:
        default:
            l = (Language *) new Lang_C_x64();
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
    b->ltraces = opt_t;
    b->lasm = opt_a;
    b->loadm = opt_m;
    b->ex = exclude;
    //b->named.insert({ 0x0040B440,"UStrClr" });
    b->named = named;
    b->Cut(path,addr);
    delete b;
    return (1);
}

const char* HELP = "\
--------------------------------------------------------------\n\
Butcher ("MY_VERSION") programed by Alex Bassas.\n\
--------------------------------------------------------------\n\
usage: Butcher [-l<lang>][-m][-a][-t][-e<addr,addr,...>][-n<file>] <path> <addr>\n\
\n\
-l<lang>    => Output language: [c|p]\n\
                    - c -> C\n\
                    - p -> Python\n\
\n\
-m                  => Load memory from the file\n\
-t                  => Enable traces\n\
-a                  => Enable asm code\n\
-e[addr,addr,...]   => Exclude addresses\n\
-n[file]            => Add named functions from <file>\n\
\n";

int main (int argc, char **argv) {
int p,i;
char path[MAX_STR];
uint64_t addr;

    while ((p = getopt(argc,argv,"mtal:e:n:")) != -1) {
        switch (p) {
                case 't':
                    opt_t = true;
                    break;
                case 'a':
                    opt_a = true;
                    break;
                case 'm':
                    opt_m = true;
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
                case 'e':
                    parse_ex(optarg);
                    break;
                case 'n':
                    parse_named(optarg);
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
