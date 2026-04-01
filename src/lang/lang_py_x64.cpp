#include "lang_py_x64.hpp"

Lang_Py_x64::Lang_Py_x64(int b) {
    bits = b;
    /*
    COMM = "#";
    COMM_SEP = 70;
    INDENT = "    ";
    ENDS = "";
    SEPS = ";";
    EQUAL = "==";
    NEQUAL = "!=";
    GT = ">";
    LT = "<";
    GTE = ">=";
    LTE = "<=";
    F_SET_ZF = "cpu.flag_z(";
    F_SET_SF = "cpu.flag_s(";
    F_SET_CF = "cpu.flag_c(";
    F_SET_OF = "cpu.flag_o(";
    F_GET_ZF = "cpu.get_flag_z(";
    F_GET_SF = "cpu.get_flag_s(";
    F_GET_CF = "cpu.get_flag_c(";
    F_GET_OF = "cpu.get_flag_o(";
    F_ADD_OF = "cpu.add_flag_o(";
    F_ADD_CF = "cpu.add_flag_c(";
    F_NUM_CF = "cpu.num_flag_c(";
    F_SUB_OF = "cpu.sub_flag_o(";
    F_IDIV = "cpu.idiv(";
    //F_SUB_CF = "cpu.sub_flag_c(";
    ENDF = ")";
    AND = "&";
    OR = "|";
    XOR = "^";
    SHL = "<<";
    SHR = ">>";
    F_TRUE = "True";
    F_FALSE = "False";
    F_PUSH = "cpu.push(";
    F_POP = "cpu.pop(";
    F_POW = "cpu.pow(";
    E_JCC_GOTO = "if %s:\n        goto .label_0x%llx";
    E_IFTHENELSE= "if %s:\n        %s\n    else:\n        %s";
    E_IFTHEN= "if %s:\n        %s";
    F_NOT = "cpu.f_not(";
    F_NEG = "cpu.f_neg(";
    F_PSHUFD = "cpu.pshufd(";
    E_NOT = "not";
    E_LABEL_NAME = "    label .label_%s\n";

    OP_ALONE =              "cpu.op(\"%s\")";
    OP_SUBNAME =            "cpu.op_%s(\"%s\",%s)";
    OP_REG = "\"%s\",";
    OP_IMM = "0x%llx,";
    OP_MEM = "\"%s\",\"%s\",%i,%+lld,";

    E_CALL_FROM_IAT =       "cpu.call_from_iat(\"%s\",\"%s\")";
    E_FUNC_NAME =           "%s(cpu)";
    E_FUNC_ADDR =           "func_0x%llx(cpu)";
    E_RETURN =              "goto .label_return";
    E_GOTO =                "goto .label_0x%llx";
    E_LABEL =               "    label .label_0x%llx\n";
    E_JMP_FROM_IAT =        "cpu.jmp_from_iat(\"%s\",\"%s\");";
    E_JE =                  "if cpu.get_flag_z():\n        goto .label_0x%llx";
    E_JNE =                 "if not cpu.get_flag_z():\n        goto .label_0x%llx";
    E_JA =                  "if not cpu.get_flag_c() and not cpu.get_flag_z():\n        goto .label_0x%llx";
    E_JAE =                 "if not cpu.get_flag_c():\n        goto .label_0x%llx";
    E_JL =                  "if cpu.get_flag_o() != cpu.get_flag_s():\n        goto .label_0x%llx";
    E_JLE =                 "if cpu.get_flag_z() or (cpu.get_flag_o() != cpu.get_flag_s()):\n        goto .label_0x%llx";
    E_JGE =                 "if cpu.get_flag_o() == cpu.get_flag_s():\n        goto .label_0x%llx";
    E_JG =                  "if (cpu.get_flag_o() == cpu.get_flag_s()) and not cpu.get_flag_z():\n        goto .label_0x%llx";
    E_JO =                  "if cpu.get_flag_o():\n        goto .label_0x%llx";
    E_JNO =                 "if not cpu.get_flag_o():\n        goto .label_0x%llx";
    E_JS =                  "if cpu.get_flag_s():\n        goto .label_0x%llx";
    E_PUSH =                "cpu.push_%s(%s)";
    E_POP =                 "%s = cpu.pop_%s()";
    E_SUB_RR =              "%s = %s - %s";
    //E_SUB_RI =              "%s = %s - %lld";
    E_ADD_RR =              "%s = %s + %s";
    E_ADD_RI =              "%s = %s + %lld";
    E_XOR_R =               "%s = 0";
    E_XOR_RR =              "%s = %s ^ %s";
    //E_XOR_RI =              "%s = %s ^ %lld";
    E_JNE_R_GOTO =          "if %s != 0:\n        goto .label_0x%llx";
    E_JE_R_GOTO =           "if %s == 0:\n        goto .label_0x%llx";
    E_JLE_R_GOTO =          "if %s <= 0:\n        goto .label_0x%llx";
    E_JA_RR_GOTO =          "if %s > %s:\n        goto .label_0x%llx";
    E_JGE_RR_GOTO=          "if %s >= %s:\n        goto .label_0x%llx";
    E_SPACE =               "";
    E_MOV_RR =              "%s = %s";
    E_MOV_RI =              "%s = 0x%llx";
    E_LEA_M =               "%s = %s%+lld";
    E_MOV_RP =              "%s = cpu.get_%s_ptr(0x%llx)";
    E_MOV_RM =              "%s = cpu.get_%s_ptr(%s)";
    E_MOV_PR =              "cpu.set_%s_ptr(0x%llx,%s)";
    E_MOV_MR =              "cpu.set_%s_ptr(%s,%s)";
    E_MOV_PI =              "cpu.set_%s_ptr(0x%llx,0x%llx)";
    E_MOV_MI =              "cpu.set_%s_ptr(%s,0x%llx)";
    E_STACK_INIT =          "    %s = 0x%llx\n    %s = %s\n";
    E_IF_R_EQ_I =           "if %s == 0x%llx:";
    E_ELIF_R_EQ_I =         "elif %s == 0x%llx:";
    E_ENDIF =               "";
    E_GET_MEM =             "cpu.get_%s_ptr(%s)";
    E_S_GET_MEM =           "cpu.s_get_%s_ptr(%s)";
    E_LOAD_MEM =            "cpu.load_mem(\"%s\",0x%llx,0x%llx,0x%llx,0x%llx)";
    E_SET_MEM =             "cpu.set_%s_ptr(%s,%s)";
    */
}

#define PY_HEADER "\
from butcher_x64 import _cpu\n\
from goto import with_goto\n\
\n"

void Lang_Py_x64::PrintHeader(Code *c) {
    printf(PY_HEADER);
}

#define ANON_CALL   "AnonCall"

void Lang_Py_x64::PrintAnonCalls(Code *c) {
int count = 0;

    printf("def " ANON_CALL "(cpu,addr):\n");
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            if (!count) {
                printf("    if addr == 0x%llx:\n",c->subcodes[n].first);
            }
            else {
                printf("    elif addr == 0x%llx:\n",c->subcodes[n].first);
            }
            if (c->subcodes[n].name != NULL) {
                printf("        %s(cpu)\n",c->subcodes[n].name);
            }
            else {
                printf("        func_0x%llx(cpu)\n",c->subcodes[n].first);
            }
            //printf("    }\n");
            count++;
        }
    }
    if (count) {
        printf("    else:\n");
        printf("        cpu.panic(\"" ANON_CALL ": \"+hex(addr))\n");
        //printf("    }\n");
    }
    //printf("}\n");
}

#define PY_FOOTER_1 "\
def main():\n\
    cpu = _cpu()\n\
"

#define PY_FOOTER_32B "\
    cpu.b32 = True\n\
"

void Lang_Py_x64::PrintMainOpen(Code *c) {
    printf(PY_FOOTER_1);
    if (bits == 32) {
        printf(PY_FOOTER_32B);
    }
}

#define PY_FOOTER_2 "\
    # Insert code here ...\n\
    func_0x%llx(cpu)\n\
    # Insert code here ...\n\
\n\
if __name__==\"__main__\":\n\
    main()\n\
\n"

void Lang_Py_x64::PrintMainClose(Code *c) {
    printf(PY_FOOTER_2,c->ep);
}

void Lang_Py_x64::PrintSubMem(Code *c,int num) {
struct _submem *sm;
char sub[128];

    sm = &c->submems[num];
    if (sm->mem != NULL) {
        char *buffer = (char *) malloc((sm->size*4)+128);
        strcpy(buffer,"b'");
        for (int n=0;n<sm->size;n++) {
            sprintf(sub,"\\x%02x",sm->mem[n]);
            strcat(buffer,sub);
        }
        strcat(buffer,"'");
        printf("    cpu.add_mem(0x%llx,%s)\n",sm->addr,buffer);
        free(buffer);
    }
    else {
        printf("    cpu.add_zero_mem(0x%llx,%i)\n",sm->addr,sm->size);
    }
}

#define PY_FUNC_HEADER_NAME "\
@with_goto\n\
def %s(cpu):\n\
"

void Lang_Py_x64::PrintFuncHeaderName(Code *c,int num,char *name) {
    printf(PY_FUNC_HEADER_NAME,name,c->subcodes[num].first);
}

#define PY_FUNC_HEADER_ADDR "\
@with_goto\n\
def func_0x%llx(cpu):\n\
"

void Lang_Py_x64::PrintFuncHeaderAddr(Code *c,int num) {
    printf(PY_FUNC_HEADER_ADDR,c->subcodes[num].first);
}

#define PY_FUNC_FOOTER "\
    label .label_return\n\
    return\n\
\n"

void Lang_Py_x64::PrintAnonJmpVar(void) {
    // None
}

void Lang_Py_x64::PrintAnonJmpCall(uint64_t addr,char *name) {
    printf("    if anon == 0x%llx:\n",addr);
    printf("        goto %s\n",name);
}

void Lang_Py_x64::PrintFuncFooter(Code *c,int num) {
    printf(PY_FUNC_FOOTER);
}

void Lang_Py_x64::PrintSubCodeSep(void) {
    printf("    # --------------------------------------------------------------\n");
}

/*
char *Lang_Py_x64::mem_str(csh handle,cs_x86_op op) {
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
*/

char *Lang_Py_x64::reg_name(csh handle,int id_reg) {
char *buffer;

    buffer = (char *) malloc(256);
    if (id_reg == X86_REG_INVALID) {
        strcpy(buffer,"");
    }
    else {
        switch (id_reg) {
            case X86_REG_ST0:
                sprintf(buffer,"cpu._st0");
                break;
            default:
                sprintf(buffer,"cpu._%s",cs_reg_name(handle,id_reg));
                break;
        }
    }
    return (buffer);
}

char *Lang_Py_x64::s_reg_name(csh handle,int id_reg) {
char *buffer;

    buffer = (char *) malloc(256);
    if (id_reg == X86_REG_INVALID) {
        strcpy(buffer,"");
    }
    else {
        sprintf(buffer,"cpu.s_%s",cs_reg_name(handle,id_reg));
    }
    return (buffer);
}

const char *Lang_Py_x64::COMM(void) {
    return ("#");
}

int Lang_Py_x64::COMM_SEP(void) {
    return (70);
}

const char *Lang_Py_x64::INDENT(void) {
    return ("    ");
}

const char *Lang_Py_x64::ENDS(void) {
    return ("");
}

const char *Lang_Py_x64::E_CALL_FROM_IAT(void) {
    return ("cpu.call_from_iat(\"%s\",\"%s\")");
}

const char *Lang_Py_x64::E_FUNC_NAME(void) {
    return ("%s(cpu)");
}

const char *Lang_Py_x64::E_FUNC_ADDR(void) {
    return ("func_0x%llx(cpu)");
}

const char *Lang_Py_x64::E_RETURN(void) {
    return ("goto .label_return");
}

const char *Lang_Py_x64::E_JMP_FROM_IAT(void) {
    return ("cpu.jmp_from_iat(\"%s\",\"%s\");");
}

const char *Lang_Py_x64::E_ENDIF(void) {
    return ("");
}

const char *Lang_Py_x64::E_SPACE(void) {
    return ("");
}

const char *Lang_Py_x64::OP_ALONE(void) {
    return ("cpu.op(\"%s\")");
}

const char *Lang_Py_x64::OP_SUBNAME(void) {
    return ("cpu.op_%s(\"%s\",%s)");
}

const char *Lang_Py_x64::OP_REG(void) {
    return ("\"%s\",");
}

const char *Lang_Py_x64::OP_IMM(void) {
    return ("0x%llx,");
}

const char *Lang_Py_x64::OP_MEM(void) {
    return ("\"%s\",\"%s\",%i,%+lld,");
}

const char *Lang_Py_x64::E_STACK_INIT(void) {
    return ("    %s = 0x%llx\n    %s = %s\n");
}

const char *Lang_Py_x64::E_IF_R_EQ_I(void) {
    return ("if %s == 0x%llx:");
}

const char *Lang_Py_x64::E_ELIF_R_EQ_I(void) {
    return ("elif %s == 0x%llx:");
}

const char *Lang_Py_x64::E_GET_MEM(void) {
    return ("cpu.get_%s_ptr(%s)");
}

const char *Lang_Py_x64::E_S_GET_MEM(void) {
    return ("cpu.s_get_%s_ptr(%s)");
}

const char *Lang_Py_x64::E_LOAD_MEM(void) {
    return ("cpu.load_mem(\"%s\",0x%llx,0x%llx,0x%llx,0x%llx)");
}

const char *Lang_Py_x64::E_SET_MEM(void) {
    return ("cpu.set_%s_ptr(%s,%s)");
}

const char *Lang_Py_x64::F_SET_ZF(void) {
    return ("cpu.flag_z(");
}

const char *Lang_Py_x64::F_SET_SF(void) {
    return ("cpu.flag_s(");
}

const char *Lang_Py_x64::F_SET_CF(void) {
    return ("cpu.flag_c(");
}

const char *Lang_Py_x64::F_SET_OF(void) {
    return ("cpu.flag_o(");
}

const char *Lang_Py_x64::F_GET_ZF(void) {
    return ("cpu.get_flag_z(");
}

const char *Lang_Py_x64::F_GET_SF(void) {
    return ("cpu.get_flag_s(");
}

const char *Lang_Py_x64::F_GET_CF(void) {
    return ("cpu.get_flag_c(");
}

const char *Lang_Py_x64::F_GET_OF(void) {
    return ("cpu.get_flag_o(");
}

const char *Lang_Py_x64::F_ADD_OF(void) {
    return ("cpu.add_flag_o(");
}

const char *Lang_Py_x64::F_ADD_CF(void) {
    return ("cpu.add_flag_c(");
}

/*
const char *Lang_Py_x64::F_NUM_CF(void) {
    return ("cpu.num_flag_c(");
}
*/

const char *Lang_Py_x64::F_SUB_OF(void) {
    return ("cpu.sub_flag_o(");
}

const char *Lang_Py_x64::F_IDIV(void) {
    return ("cpu.idiv(");
}

const char *Lang_Py_x64::EQUAL(void) {
    return ("==");
}

const char *Lang_Py_x64::NEQUAL(void) {
    return ("!=");
}

const char *Lang_Py_x64::GT(void) {
    return (">");
}

const char *Lang_Py_x64::LT(void) {
    return ("<");
}

const char *Lang_Py_x64::GTE(void) {
    return (">=");
}

const char *Lang_Py_x64::LTE(void) {
    return ("<=");
}

const char *Lang_Py_x64::ENDF(void) {
    return (")");
}

const char *Lang_Py_x64::AND(void) {
    return ("&");
}

const char *Lang_Py_x64::OR(void) {
    return ("|");
}

const char *Lang_Py_x64::XOR(void) {
    return ("^");
}

const char *Lang_Py_x64::SHL(void) {
    return ("<<");
}

const char *Lang_Py_x64::SHR(void) {
    return (">>");
}

const char *Lang_Py_x64::F_TRUE(void) {
    return ("True");
}

const char *Lang_Py_x64::F_FALSE(void) {
    return ("False");
}

const char *Lang_Py_x64::F_PUSH(void) {
    return ("cpu.push(");
}

const char *Lang_Py_x64::F_POP(void) {
    return ("cpu.pop(");
}

const char *Lang_Py_x64::F_POW(void) {
    return ("cpu.pow(");
}

const char *Lang_Py_x64::E_IFTHENELSE(void) {
    return ("if %s:\n%s\nelse:\n%s");
}

const char *Lang_Py_x64::E_IFTHEN(void) {
    return ("if %s:\n%s");
}

const char *Lang_Py_x64::F_NOT(void) {
    return ("cpu.f_not(");
}

const char *Lang_Py_x64::F_NEG(void) {
    return ("cpu.f_neg(");
}

const char *Lang_Py_x64::F_PSHUFD(void) {
    return ("cpu.pshufd(");
}

const char *Lang_Py_x64::E_NOT(void) {
    return ("not");
}

const char *Lang_Py_x64::E_LABEL_NAME(void) {
    return (".label_%s");
}

const char *Lang_Py_x64::E_LABEL_ADDR(void) {
    return (".label_0x%llx");
}

const char *Lang_Py_x64::E_LABEL(void) {
    return ("    label %s\n");
}

const char *Lang_Py_x64::E_LABEL_ANON(void) {
    return ("    label .label_Anon\n");
}

const char *Lang_Py_x64::E_GOTO(void) {
    return ("goto %s");
}

const char *Lang_Py_x64::E_JCC_GOTO(void) {
    return ("if %s: goto %s");
}

const char *Lang_Py_x64::E_ANONC(void) {
    return (ANON_CALL "(");
}

const char *Lang_Py_x64::F_PUSHFPU(void) {
    return ("cpu.pushfpu(");
}

const char *Lang_Py_x64::F_POPFPU(void) {
    return ("cpu.popfpu(");
}

const char *Lang_Py_x64::E_ANONJ(void) {
    return ("anon");
}

const char *Lang_Py_x64::E_LABEL_ANONJ(void) {
    return (".label_Anon");
}

const char *Lang_Py_x64::E_WHILE(void) {
    return ("while %s:\n%s");
}

const char *Lang_Py_x64::E_BREAK(void) {
    return ("break");
}