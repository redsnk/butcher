#include "lang_c_x64.hpp"

Lang_C_x64::Lang_C_x64() {
    COMM = "//";
    COMM_SEP = 70;
    INDENT = "    ";
    ENDS = ";";
    SEPS = ";";
    EQUAL = "==";
    NEQUAL = "!=";
    GT = ">";
    LT = "<";
    GTE = ">=";
    LTE = "<=";
    F_ZF = "set_flag_z(cpu,";
    F_SF = "set_flag_s(cpu,";
    F_CF = "set_flag_c(cpu,";
    F_OF = "set_flag_o(cpu,";
    F_ADD_OF = "add_flag_o(cpu,";
    F_ADD_CF = "add_flag_c(cpu,";
    F_SUB_OF = "sub_flag_o(cpu,";
    //F_SUB_CF = "sub_flag_c(cpu,";
    ENDF = ")";
    AND = "&";
    OR = "|";
    XOR = "^";
    F_TRUE = "1";
    F_FALSE = "0";
    F_PUSH = "push(cpu,";
    F_POP = "pop(cpu,";
    F_POW = "Pow(";

    OP_ALONE =              "op(cpu,\"%s\");";
    OP_SUBNAME =            "op_%s(cpu,\"%s\",%s);";
    OP_REG = "\"%s\",";
    OP_IMM = "0x%llx,";
    OP_MEM = "\"%s\",\"%s\",%i,%+lld,";

    E_CALL_FROM_IAT =       "call_from_iat(cpu,\"%s\",\"%s\");";
    E_FUNC_NAME =           "%s(cpu);";
    E_FUNC_ADDR =           "func_0x%llx(cpu);";
    E_RETURN =              "return;";
    E_GOTO =                "goto label_0x%llx;";
    E_LABEL =               "label_0x%llx:\n";
    E_JMP_FROM_IAT =        "jmp_from_iat(cpu,\"%s\",\"%s\");";
    E_JE =                  "if (flag_z(cpu)) goto label_0x%llx;";
    E_JNE =                 "if (!flag_z(cpu)) goto label_0x%llx;";
    E_JA =                  "if (!flag_c(cpu) && !flag_z(cpu)) goto label_0x%llx;";
    E_JAE =                 "if (!flag_c(cpu)) goto label_0x%llx;";
    E_JL =                  "if (flag_o(cpu) != flag_s(cpu)) goto label_0x%llx;";
    E_JLE =                 "if (flag_z(cpu) || (flag_o(cpu) != flag_s(cpu))) goto label_0x%llx;";
    E_JGE =                 "if (flag_o(cpu) == flag_s(cpu)) goto label_0x%llx;";
    E_JO =                  "if (flag_o(cpu)) goto label_0x%llx;";
    E_PUSH =                "_push_%s(%s);";
    E_POP =                 "%s = _pop_%s();";
    E_SUB_RR =              "%s = %s - %s;";
    //E_SUB_RI =              "%s = %s - %lld;";
    E_ADD_RR =              "%s = %s + %s;";
    E_ADD_RI =              "%s = %s + %lld;";
    E_XOR_R =               "%s = 0;";
    E_XOR_RR =              "%s = %s ^ %s;";
    //E_XOR_RI =              "%s = %s ^ %lld;";
    E_JNE_R_GOTO =          "if (%s != 0) goto label_0x%llx;";
    E_JE_R_GOTO =           "if (%s == 0) goto label_0x%llx;";
    E_JLE_R_GOTO =          "if (%s <= 0) goto label_0x%llx;";
    E_JA_RR_GOTO =          "if (%s > %s) goto label_0x%llx;";
    E_JGE_RR_GOTO =         "if (%s >= %s) goto label_0x%llx;";
    E_SPACE =               "";
    E_MOV_RR =              "%s = %s;";
    E_MOV_RI =              "%s = 0x%llx;";
    E_LEA_M =               "%s = %s%+lld;";
    E_MOV_RP =              "%s = _get_%s_ptr(0x%llx);";
    E_MOV_RM =              "%s = _get_%s_ptr(%s);";
    E_MOV_PR =              "_set_%s_ptr(0x%llx,%s);";
    E_MOV_MR =              "_set_%s_ptr(%s,%s);";
    E_MOV_PI =              "_set_%s_ptr(0x%llx,0x%llx);";
    E_MOV_MI =              "_set_%s_ptr(%s,0x%llx);";
    E_STACK_INIT =          "    %s = 0x%llx;\n    %s = %s;\n";
    E_IF_R_EQ_I =           "if (%s == 0x%llx) {";
    E_ELIF_R_EQ_I =         "else if (%s == 0x%llx) {";
    E_ENDIF =               "}";
    E_GET_MEM =             "_get_%s_ptr(%s)";
    E_S_GET_MEM =           "s_get_%s_ptr(%s)";
    E_LOAD_MEM =            "load_mem(cpu,\"%s\",0x%llx,0x%llx,0x%llx,0x%llx);";
    E_SET_MEM =             "_set_%s_ptr(%s,%s)";
}

#define C_HEADER "\
#include \"butcher_x64.h\"\n\
\n"

#define C_FUNC_ADDR "\
void func_0x%llx(struct _cpu *cpu);\n"

#define C_FUNC_NAME "\
void %s(struct _cpu *cpu);    // 0x%llx\n"


void Lang_C_x64::PrintHeader(Code *c) {
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
struct _cpu c,*cpu;\n\
\n\
    cpu = &c;\n\
    init(cpu);\n\
"

void Lang_C_x64::PrintMainOpen(Code *c) {
    printf(C_FOOTER_1);
}

#define C_FOOTER_2 "\
    /* Insert code here ... */\n\
    func_0x%llx(cpu);\n\
    /* Insert code here ... */\n\
    end(cpu);\n\
    return (0);\n\
}\n\
\n"

void Lang_C_x64::PrintMainClose(Code *c) {
    printf(C_FOOTER_2,c->ep);
}

void Lang_C_x64::PrintSubMem(Code *c,int num) {
struct _submem *sm;
char sub[128];

    sm = &c->submems[num];
    if (sm->mem != NULL) {
        char *buffer = (char *) malloc((sm->size*4)+128);
        buffer[0] = 0;
        for (int n=0;n<sm->size;n++) {
            sprintf(sub,"\\x%02x",sm->mem[n]);
            strcat(buffer,sub);
        }
        printf("    add_mem(cpu,0x%llx,\"%s\",%i);\n",sm->addr,buffer,sm->size);
        free(buffer);
    }
    else {
        printf("    add_mem(cpu,0x%llx,NULL,%i);\n",sm->addr,sm->size);
    }
}

#define C_FUNC_HEADER_NAME "\
void %s(struct _cpu *cpu) {\n\
"

void Lang_C_x64::PrintFuncHeaderName(Code *c,int num,char *name) {
    printf(C_FUNC_HEADER_NAME,name,c->subcodes[num].first);
}

#define C_FUNC_HEADER_ADDR "\
void func_0x%llx(struct _cpu *cpu) {\n\
"

void Lang_C_x64::PrintFuncHeaderAddr(Code *c,int num) {
    printf(C_FUNC_HEADER_ADDR,c->subcodes[num].first);
}

#define C_FUNC_FOOTER "\
}\n\
\n"

void Lang_C_x64::PrintFuncFooter(Code *c,int num) {
    printf(C_FUNC_FOOTER);
}

void Lang_C_x64::PrintSubCodeSep(void) {
    printf("    // --------------------------------------------------------------\n");
}

char *Lang_C_x64::mem_str(csh handle,cs_x86_op op) {
char tmp[256];
char *buffer;

    // mov		ecx, dword ptr [r8 + rax*4 + 0x27b8]
    buffer = (char *) malloc(256);
    if (op.mem.base != X86_REG_INVALID) {
        sprintf(buffer,"%s",reg_name(handle,op.mem.base));
    }
    else {
        buffer[0] = 0;
    }
    if (op.mem.index != X86_REG_INVALID) {
        sprintf(tmp,"+%s*%i",reg_name(handle,op.mem.index),op.mem.scale);
        strcat (buffer,tmp);
    }
    if (op.mem.disp) {
        sprintf(tmp,"%+lld",op.mem.disp);
        strcat (buffer,tmp);
    }
    return (buffer);
}

char *Lang_C_x64::reg_name(csh handle,int id_reg) {
char *buffer;

    buffer = (char *) malloc(256);
    if (id_reg == X86_REG_INVALID) {
        strcpy(buffer,"");
    }
    else {
        sprintf(buffer,"_%s",cs_reg_name(handle,id_reg));
    }
    return (buffer);
}

char *Lang_C_x64::s_reg_name(csh handle,int id_reg) {
char *buffer;

    buffer = (char *) malloc(256);
    if (id_reg == X86_REG_INVALID) {
        strcpy(buffer,"");
    }
    else {
        sprintf(buffer,"s_%s",cs_reg_name(handle,id_reg));
    }
    return (buffer);
}


