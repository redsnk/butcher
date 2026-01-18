#include <stdio.h>
#include <unistd.h>
#include "src/arch/pe_x64.hpp"

#define MY_VERSION  "v0.01"

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
    Butcher *b = new Pe_x64();
    if (!b->CheckFile(path)) {
        delete b;
        printf("Error: format not compatible.");
        return (0);
    }
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
