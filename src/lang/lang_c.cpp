#include "lang_c.hpp"

Lang_C::Lang_C() {
    COMM = "//";
    COMM_SEP = 70;
}

#define C_HEADER "\
#include \"butcher_x64.h\"\n\
\n"

#define C_FUNC_ADDR "\
void func_0x%llx(struct _cpu *cpu);\n"

#define C_FUNC_NAME "\
void %s(struct _cpu *cpu);    // 0x%llx\n"


void Lang_C::PrintHeader(Code *c) {
    printf(C_HEADER);
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            if (c->subcodes[n].name != NULL) {
                printf(C_FUNC_NAME,c->subcodes[n].name,c->subcodes[n].first);
            }
            else {
                printf(C_FUNC_ADDR,c->subcodes[n].first);
            }
        }
    } 
}

#define C_FOOTER_1 "\
int main (int argc, char **argv) {\n\
struct _cpu cpu;\n\
\n\
    init(&cpu);\n\
"

void Lang_C::PrintMainOpen(Code *c) {
    printf(C_FOOTER_1);
}

#define C_FOOTER_2 "\
    func_0x%llx(&cpu);\n\
    end(&cpu);\n\
    return (0);\n\
}\n\
\n"

void Lang_C::PrintMainClose(Code *c) {
    printf(C_FOOTER_2,c->ep);
}

void Lang_C::PrintSubMem(Code *c,int num) {
struct _submem *sm;
char sub[128];

    sm = &c->submems[num];
    char *buffer = (char *) malloc((sm->size*4)+128);
    buffer[0] = 0;
    for (int n=0;n<sm->size;n++) {
        sprintf(sub,"\\x%02x",sm->mem[n]);
        strcat(buffer,sub);
    }
    printf("    add_mem (&cpu,0x%llx,\"%s\",%i);\n",sm->addr,buffer,sm->size);
    free(buffer);
}

#define C_FUNC_HEADER_NAME "\
void %s(struct _cpu *cpu) {\n\
"

void Lang_C::PrintFuncHeaderName(Code *c,int num,char *name) {
    printf(C_FUNC_HEADER_NAME,name,c->subcodes[num].first);
}

#define C_FUNC_HEADER_ADDR "\
void func_0x%llx(struct _cpu *cpu) {\n\
"

void Lang_C::PrintFuncHeaderAddr(Code *c,int num) {
    printf(C_FUNC_HEADER_ADDR,c->subcodes[num].first);
}

#define C_FUNC_FOOTER "\
}\n\
\n"

void Lang_C::PrintFuncFooter(Code *c,int num) {
    printf(C_FUNC_FOOTER);
}

void Lang_C::PrintSubCodeSep(void) {
    printf("    // --------------------------------------------------------------\n");
}