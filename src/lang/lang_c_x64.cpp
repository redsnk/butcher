#include "lang_c_x64.hpp"

Lang_C_x64::Lang_C_x64() {
}

#define C_HEADER "\
#include \"butcher_x64.h\"\n\
\n"

#define C_FUNC_ADDR "\
void func_0x%llx(struct _cpu *cpu,uint64_t raddr);\n"

#define C_FUNC_NAME "\
void %s(struct _cpu *cpu,uint64_t raddr);    // 0x%llx\n"

#define ANON_CALL   "AnonCall"

void Lang_C_x64::PrintHeader(Code *c) {
    PrintF(C_HEADER);
    PrintF("void " ANON_CALL "(struct _cpu *cpu,uint64_t addr,uint64_t raddr);\n");
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            if (c->subcodes[n].name != NULL) {
                PrintF(C_FUNC_NAME,c->subcodes[n].name,c->subcodes[n].first);
            }
            else {
                PrintF(C_FUNC_ADDR,c->subcodes[n].first);
            }
        }
    } 
}

void Lang_C_x64::PrintAnonCalls(Code *c) {
int count = 0;

    PrintF("void " ANON_CALL "(struct _cpu *cpu,uint64_t addr,uint64_t raddr) {\n");
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            if (!count) {
                PrintF("    if (addr == 0x%llx) {\n",c->subcodes[n].first);
            }
            else {
                PrintF("    else if (addr == 0x%llx) {\n",c->subcodes[n].first);
            }
            if (c->subcodes[n].name != NULL) {
                PrintF("        %s(cpu,raddr);\n",c->subcodes[n].name);
            }
            else {
                PrintF("        func_0x%llx(cpu,raddr);\n",c->subcodes[n].first);
            }
            PrintF("    }\n");
            count++;
        }
    }
    if (count) {
        PrintF("    else {\n");
        PrintF("        panic(cpu,\"CALL\",\"0x%%llx\",addr);\n");
        PrintF("    }\n");
    }
    PrintF("}\n");
}

#define C_FOOTER_1 "\
int main (int argc, char **argv) {\n\
struct _cpu c,*cpu;\n\
\n\
    cpu = &c;\n\
    init(cpu);\n\
"

void Lang_C_x64::PrintMainOpen(Code *c) {
    PrintF(C_FOOTER_1);
}

#define C_FOOTER_2 "\
    /* Insert code here ... */\n"

#define C_FOOTER_F "\
    func_0x%llx(cpu,0);\n"

#define C_FOOTER_N "\
    %s(cpu,0);\n"

#define C_FOOTER_3 "\
    /* Insert code here ... */\n\
    end(cpu);\n\
    return (%i);\n\
}\n\
\n"

void Lang_C_x64::PrintMainClose(Code *c,char *name) {
    PrintF(C_FOOTER_2);
    if (name != NULL) {
        PrintF(C_FOOTER_N,name);
    }
    else {
        PrintF(C_FOOTER_F,c->ep);
    }
    PrintF(C_FOOTER_3,ERR_CODE_OK);
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
        PrintF("    add_mem(cpu,0x%llx,\"%s\",%i);\n",sm->addr,buffer,sm->size);
        free(buffer);
    }
    else {
        PrintF("    add_mem(cpu,0x%llx,NULL,%i);\n",sm->addr,sm->size);
    }
}

#define C_FUNC_HEADER_NAME "\
void %s(struct _cpu *cpu,uint64_t raddr) {\n\
"

void Lang_C_x64::PrintFuncHeaderName(Code *c,int num,char *name) {
    PrintF(C_FUNC_HEADER_NAME,name,c->subcodes[num].first);
}

#define C_FUNC_HEADER_ADDR "\
void func_0x%llx(struct _cpu *cpu,uint64_t raddr) {\n\
"

void Lang_C_x64::PrintFuncHeaderAddr(Code *c,int num) {
    PrintF(C_FUNC_HEADER_ADDR,c->subcodes[num].first);
}

#define C_FUNC_FOOTER "\
}\n\
\n"

void Lang_C_x64::PrintAnonJmpVar(void) {
    PrintF("uint64_t anon;\n\n");
}

void Lang_C_x64::PrintAnonJmpCall(uint64_t addr,char *name) {
    PrintF("    if (anon == 0x%llx) {\n",addr);
    PrintF("        goto %s;\n",name);
    PrintF("    }\n"); 
}

void Lang_C_x64::PrintAnonJmpEnd(void) {
    PrintF("    if (anon == raddr) {\n");
    PrintF("        return;\n");
    PrintF("    }\n");
    PrintF("    panic(cpu,\"JMP\",\"0x%%llx\",raddr);\n");
}

void Lang_C_x64::PrintFuncFooter(Code *c,int num) {
    PrintF(C_FUNC_FOOTER);
}

void Lang_C_x64::PrintSubCodeSep(void) {
    PrintF("    // --------------------------------------------------------------\n");
}

void Lang_C_x64::PrintLoadError(const char *code,int num,const char *msg) {
    PrintF("    cpu->errors[cpu->num_errors].code = \"%s\";\n",code);
    PrintF("    cpu->errors[cpu->num_errors].num = %i;\n",num);
    PrintF("    cpu->errors[cpu->num_errors++].msg = \"%s\";\n",msg);
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
            case X86_REG_ST1:
                sprintf(buffer,"_st1");
                break;
            case X86_REG_ST2:
                sprintf(buffer,"_st2");
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
    return ("%s(cpu,0x%llx);");
}

const char *Lang_C_x64::E_FUNC_ADDR(void) {
    return ("func_0x%llx(cpu,0x%llx);");
}

/*
const char *Lang_C_x64::E_RETURN(void) {
    return ("return;");
}
*/

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

const char *Lang_C_x64::F_SET_DF(void) {
    return ("set_flag_d(cpu,");
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

const char *Lang_C_x64::F_GET_DF(void) {
    return ("flag_d(cpu");
}

const char *Lang_C_x64::F_ADD_OF(void) {
    return ("add_flag_o(cpu,");
}

const char *Lang_C_x64::F_ADD_CF(void) {
    return ("add_flag_c(cpu,");
}

/*
const char *Lang_C_x64::F_NUM_CF(void) {
    return ("num_flag_c(cpu");
}
*/

const char *Lang_C_x64::F_SUB_OF(void) {
    return ("sub_flag_o(cpu,");
}

const char *Lang_C_x64::F_SDIV(void) {
    return ("sdiv(");
}

const char *Lang_C_x64::F_UDIV(void) {
    return ("udiv(");
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
/*
const char *Lang_C_x64::F_POW(void) {
    return ("Pow(");
}
*/

const char *Lang_C_x64::E_IFTHENELSE(void) {
    return ("if(%s) {\n%s;\n}\nelse {\n%s;\n}");
}

const char *Lang_C_x64::E_IFTHEN(void) {
    return ("if(%s) {\n%s;\n}");
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

const char *Lang_C_x64::E_ANONJ(void) {
    return ("anon");
}

const char *Lang_C_x64::E_LABEL_ANONJ(void) {
    return ("label_Anon");
}

const char *Lang_C_x64::E_WHILE(void) {
    return ("while(%s) {\n%s;\n}");
}

const char *Lang_C_x64::E_BREAK(void) {
    return ("break");
}

const char *Lang_C_x64::E_TMP(void) {
    return ("_tmp");
}

const char *Lang_C_x64::E_TMP2(void) {
    return ("_tmp2");
}

const char *Lang_C_x64::F_MASK(void) {
    return ("mask(");
}

const char *Lang_C_x64::F_UTOD(void) {
    return ("utod(");
}

const char *Lang_C_x64::F_DTOU(void) {
     return ("dtou(");
}

const char *Lang_C_x64::F_UTOF(void) {
    return ("utof(");
}

const char *Lang_C_x64::F_FTOU(void) {
     return ("ftou(");
}

const char *Lang_C_x64::F_UTOL(void) {
    return ("utol(");
}

const char *Lang_C_x64::F_LTOU(void) {
     return ("ltou(");
}

const char *Lang_C_x64::E_EFLAGS(void) {
     return ("cpu->eflags.r32");
}

const char *Lang_C_x64::E_ELSE(void) {
     return ("else {");
}

const char *Lang_C_x64::E_PANIC_JMP_INDEX(void) {
    return ("panic(cpu,\"JMP\",\"%s\");");
}

