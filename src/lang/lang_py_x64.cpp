#include "lang_py_x64.hpp"

Lang_Py_x64::Lang_Py_x64() {
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
    F_SUB_OF = "cpu.sub_flag_o(";
    //F_SUB_CF = "cpu.sub_flag_c(";
    ENDF = ")";
    AND = "&";
    OR = "|";
    XOR = "^";
    F_TRUE = "true";
    F_FALSE = "false";
    F_PUSH = "cpu.push(";
    F_POP = "cpu.pop(";
    F_POW = "cpu.pow(";
    E_JCC_GOTO = "if %s:\n        goto .label_0x%llx";
    E_IFTHENELSE= "if %s:\n        %s\n    else:\n        %s";
    E_IFTHEN= "if %s:\n        %s";
    F_NOT = "cpu.not(";
    F_NEG = "cpu.neg(";

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
    E_JE =                  "if cpu.flag_z():\n        goto .label_0x%llx";
    E_JNE =                 "if not cpu.flag_z():\n        goto .label_0x%llx";
    E_JA =                  "if not cpu.flag_c() and not cpu.flag_z():\n        goto .label_0x%llx";
    E_JAE =                 "if not cpu.flag_c():\n        goto .label_0x%llx";
    E_JL =                  "if cpu.flag_o() != cpu.flag_s():\n        goto .label_0x%llx";
    E_JLE =                 "if cpu.flag_z() or (cpu.flag_o() != cpu.flag_s()):\n        goto .label_0x%llx";
    E_JGE =                 "if cpu.flag_o() == cpu.flag_s():\n        goto .label_0x%llx";
    E_JG =                  "if (cpu.flag_o() == cpu.flag_s()) and not cpu.flag_z():\n        goto .label_0x%llx";
    E_JO =                  "if cpu.flag_o():\n        goto .label_0x%llx";
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
}

#define PY_HEADER "\
from butcher_x64 import _cpu\n\
from goto import with_goto\n\
\n"

void Lang_Py_x64::PrintHeader(Code *c) {
    printf(PY_HEADER);
}

#define PY_FOOTER_1 "\
def main():\n\
    cpu = _cpu()\n\
"

void Lang_Py_x64::PrintMainOpen(Code *c) {
    printf(PY_FOOTER_1);
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
        sprintf(buffer,"cpu._%s",cs_reg_name(handle,id_reg));
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
