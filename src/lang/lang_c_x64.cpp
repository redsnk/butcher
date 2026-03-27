#include "lang_c_x64.hpp"

Lang_C_x64::Lang_C_x64() {
    /*
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
    F_SET_ZF = "set_flag_z(cpu,";
    F_SET_SF = "set_flag_s(cpu,";
    F_SET_CF = "set_flag_c(cpu,";
    F_SET_OF = "set_flag_o(cpu,";
    F_GET_ZF = "flag_z(cpu";
    F_GET_SF = "flag_s(cpu";
    F_GET_CF = "flag_c(cpu";
    F_GET_OF = "flag_o(cpu";
    F_ADD_OF = "add_flag_o(cpu,";
    F_ADD_CF = "add_flag_c(cpu,";
    F_NUM_CF = "num_flag_c(cpu";
    F_SUB_OF = "sub_flag_o(cpu,";
    F_IDIV = "idiv(";
    //F_SUB_CF = "sub_flag_c(cpu,";
    ENDF = ")";
    AND = "&";
    OR = "|";
    XOR = "^";
    SHL = "<<";
    SHR = ">>";
    F_TRUE = "1";
    F_FALSE = "0";
    F_PUSH = "push(cpu,";
    F_POP = "pop(cpu,";
    F_POW = "Pow(";
    E_JCC_GOTO = "if (%s) goto label_0x%llx;";
    E_IFTHENELSE= "if (%s) { %s; } else  { %s; }";
    E_IFTHEN= "if (%s) { %s; }";
    F_NOT = "not(";
    F_NEG = "neg(";
    F_PSHUFD = "pshufd(";
    E_NOT = "!";
    E_LABEL_NAME = "label_%s:\n";
 
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
    E_JG =                  "if ((flag_o(cpu) == flag_s(cpu)) && !flag_z(cpu)) goto label_0x%llx;";
    E_JO =                  "if (flag_o(cpu)) goto label_0x%llx;";
    E_JNO =                 "if (!flag_o(cpu)) goto label_0x%llx;";
    E_JS =                  "if (flag_s(cpu)) goto label_0x%llx;";
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
    */
}

#define C_HEADER "\
#include \"butcher_x64.h\"\n\
\n"

#define C_FUNC_ADDR "\
void func_0x%llx(struct _cpu *cpu);\n"

#define C_FUNC_NAME "\
void %s(struct _cpu *cpu);    // 0x%llx\n"

#define ANON_CALL   "AnonCall"

void Lang_C_x64::PrintHeader(Code *c) {
    printf(C_HEADER);
    printf("void " ANON_CALL "(struct _cpu *cpu,uint64_t addr);\n");
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

void Lang_C_x64::PrintAnonCalls(Code *c) {
int count = 0;

    printf("void " ANON_CALL "(struct _cpu *cpu,uint64_t addr) {\n");
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            if (!count) {
                printf("    if (addr == 0x%llx) {\n",c->subcodes[n].first);
            }
            else {
                printf("    else if (addr == 0x%llx) {\n",c->subcodes[n].first);
            }
            if (c->subcodes[n].name != NULL) {
                printf("        %s(cpu);\n",c->subcodes[n].name);
            }
            else {
                printf("        func_0x%llx(cpu);\n",c->subcodes[n].first);
            }
            printf("    }\n");
            count++;
        }
    }
    if (count) {
        printf("    else {\n");
        printf("        char buffer[256];\n");
        printf("        sprintf(buffer,\"0x%%llx\",addr);\n");
        printf("        panic(\"" ANON_CALL "\",buffer);\n");
        printf("    }\n");
    }
    printf("}\n");
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

char *Lang_C_x64::reg_name(csh handle,int id_reg) {
char *buffer;

    buffer = (char *) malloc(256);
    if (id_reg == X86_REG_INVALID) {
        strcpy(buffer,"");
    }
    else {
        switch (id_reg) {
            case X86_REG_ST0:
                sprintf(buffer,"_st0");
                break;
            default:
                sprintf(buffer,"_%s",cs_reg_name(handle,id_reg));
                break;
        }
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
        switch (id_reg) {
            case X86_REG_ST0:
                sprintf(buffer,"s_st0");
                break;
            default:
                sprintf(buffer,"s_%s",cs_reg_name(handle,id_reg));
                break;
        }
    }
    return (buffer);
}

const char *Lang_C_x64::COMM(void) {
    return ("//");
}

int Lang_C_x64::COMM_SEP(void) {
    return (70);
}

const char *Lang_C_x64::INDENT(void) {
    return ("    ");
}

const char *Lang_C_x64::ENDS(void) {
    return (";");
}

const char *Lang_C_x64::E_CALL_FROM_IAT(void) {
    return ("call_from_iat(cpu,\"%s\",\"%s\");");
}

const char *Lang_C_x64::E_FUNC_NAME(void) {
    return ("%s(cpu);");
}

const char *Lang_C_x64::E_FUNC_ADDR(void) {
    return ("func_0x%llx(cpu);");
}

const char *Lang_C_x64::E_RETURN(void) {
    return ("return;");
}

const char *Lang_C_x64::E_JMP_FROM_IAT(void) {
    return ("jmp_from_iat(cpu,\"%s\",\"%s\");");
}

const char *Lang_C_x64::E_ENDIF(void) {
    return ("}");
}

const char *Lang_C_x64::E_SPACE(void) {
    return ("");
}

const char *Lang_C_x64::OP_ALONE(void) {
    return ("op(cpu,\"%s\");");
}

const char *Lang_C_x64::OP_SUBNAME(void) {
    return ("op_%s(cpu,\"%s\",%s);");
}

const char *Lang_C_x64::OP_REG(void) {
    return ("\"%s\",");
}

const char *Lang_C_x64::OP_IMM(void) {
    return ("0x%llx,");
}

const char *Lang_C_x64::OP_MEM(void) {
    return ("\"%s\",\"%s\",%i,%+lld,");
}

const char *Lang_C_x64::E_STACK_INIT(void) {
    return ("    %s = 0x%llx;\n    %s = %s;\n");
}

const char *Lang_C_x64::E_IF_R_EQ_I(void) {
    return ("if (%s == 0x%llx) {");
}

const char *Lang_C_x64::E_ELIF_R_EQ_I(void) {
    return ("else if (%s == 0x%llx) {");
}

const char *Lang_C_x64::E_GET_MEM(void) {
    return ("_get_%s_ptr(%s)");
}

const char *Lang_C_x64::E_S_GET_MEM(void) {
    return ("s_get_%s_ptr(%s)");
}

const char *Lang_C_x64::E_LOAD_MEM(void) {
    return ("load_mem(cpu,\"%s\",0x%llx,0x%llx,0x%llx,0x%llx);");
}

const char *Lang_C_x64::E_SET_MEM(void) {
    return ("_set_%s_ptr(%s,%s)");
}

const char *Lang_C_x64::F_SET_ZF(void) {
    return ("set_flag_z(cpu,");
}

const char *Lang_C_x64::F_SET_SF(void) {
    return ("set_flag_s(cpu,");
}

const char *Lang_C_x64::F_SET_CF(void) {
    return ("set_flag_c(cpu,");
}

const char *Lang_C_x64::F_SET_OF(void) {
    return ("set_flag_o(cpu,");
}

const char *Lang_C_x64::F_GET_ZF(void) {
    return ("flag_z(cpu");
}

const char *Lang_C_x64::F_GET_SF(void) {
    return ("flag_s(cpu");
}

const char *Lang_C_x64::F_GET_CF(void) {
    return ("flag_c(cpu");
}

const char *Lang_C_x64::F_GET_OF(void) {
    return ("flag_o(cpu");
}

const char *Lang_C_x64::F_ADD_OF(void) {
    return ("add_flag_o(cpu,");
}

const char *Lang_C_x64::F_ADD_CF(void) {
    return ("add_flag_c(cpu,");
}

const char *Lang_C_x64::F_NUM_CF(void) {
    return ("num_flag_c(cpu");
}

const char *Lang_C_x64::F_SUB_OF(void) {
    return ("sub_flag_o(cpu,");
}

const char *Lang_C_x64::F_IDIV(void) {
    return ("idiv(");
}

const char *Lang_C_x64::EQUAL(void) {
    return ("==");
}

const char *Lang_C_x64::NEQUAL(void) {
    return ("!=");
}

const char *Lang_C_x64::GT(void) {
    return (">");
}

const char *Lang_C_x64::LT(void) {
    return ("<");
}

const char *Lang_C_x64::GTE(void) {
    return (">=");
}

const char *Lang_C_x64::LTE(void) {
    return ("<=");
}

const char *Lang_C_x64::ENDF(void) {
    return (")");
}

const char *Lang_C_x64::AND(void) {
    return ("&");
}

const char *Lang_C_x64::OR(void) {
    return ("|");
}

const char *Lang_C_x64::XOR(void) {
    return ("^");
}

const char *Lang_C_x64::SHL(void) {
    return ("<<");
}

const char *Lang_C_x64::SHR(void) {
    return (">>");
}

const char *Lang_C_x64::F_TRUE(void) {
    return ("1");
}

const char *Lang_C_x64::F_FALSE(void) {
    return ("0");
}

const char *Lang_C_x64::F_PUSH(void) {
    return ("push(cpu,");
}

const char *Lang_C_x64::F_POP(void) {
    return ("pop(cpu,");
}

const char *Lang_C_x64::F_POW(void) {
    return ("Pow(");
}

const char *Lang_C_x64::E_IFTHENELSE(void) {
    return ("if (%s) { %s; } else  { %s; }");
}

const char *Lang_C_x64::E_IFTHEN(void) {
    return ("if (%s) { %s; }");
}

const char *Lang_C_x64::F_NOT(void) {
    return ("not(");
}

const char *Lang_C_x64::F_NEG(void) {
    return ("neg(");
}

const char *Lang_C_x64::F_PSHUFD(void) {
    return ("pshufd(");
}

const char *Lang_C_x64::E_NOT(void) {
    return ("!");
}

const char *Lang_C_x64::E_LABEL_NAME(void) {
    return ("label_%s");
}

const char *Lang_C_x64::E_LABEL_ADDR(void) {
    return ("label_0x%llx");
}
const char *Lang_C_x64::E_LABEL(void) {
    return ("%s:\n");
}

const char *Lang_C_x64::E_LABEL_ANON(void) {
    return ("label_Anon:\n");
}

const char *Lang_C_x64::E_GOTO(void) {
    return ("goto %s;");
}

const char *Lang_C_x64::E_JCC_GOTO(void) {
    return ("if (%s) goto %s;");
}

const char *Lang_C_x64::E_ANONC(void) {
    return (ANON_CALL "(cpu,");
}

const char *Lang_C_x64::F_PUSHFPU(void) {
    return ("pushfpu(cpu,");
}

const char *Lang_C_x64::F_POPFPU(void) {
    return ("popfpu(cpu");
}
