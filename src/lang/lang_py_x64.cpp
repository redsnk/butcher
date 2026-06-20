#include "lang_py_x64.hpp"

Lang_Py_x64::Lang_Py_x64(int b) {
    bits = b;
}

#define PY_HEADER "\
import sys\n\
from butcher_x64 import _cpu\n\
from goto import with_goto\n\
\n"

void Lang_Py_x64::PrintHeader(Code *c) {
    PrintF(PY_HEADER);
}

#define ANON_CALL   "AnonCall"

void Lang_Py_x64::PrintAnonCalls(Code *c) {
int count = 0;

    PrintF("def " ANON_CALL "(cpu,addr,raddr):\n");
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            if (!count) {
                PrintF("    if addr == 0x%llx:\n",c->subcodes[n].first);
            }
            else {
                PrintF("    elif addr == 0x%llx:\n",c->subcodes[n].first);
            }
            if (c->subcodes[n].name != NULL) {
                PrintF("        %s(cpu,raddr)\n",c->subcodes[n].name);
            }
            else {
                PrintF("        func_0x%llx(cpu,raddr)\n",c->subcodes[n].first);
            }
            //printf("    }\n");
            count++;
        }
    }
    if (count) {
        PrintF("    else:\n");
        PrintF("        cpu.panic(cpu,\"CALL\",\"0x%%llx\",addr)\n");
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
    PrintF(PY_FOOTER_1);
    if (bits == 32) {
        PrintF(PY_FOOTER_32B);
    }
}

#define PY_FOOTER_2 "\
    # Insert code here ...\n"

#define PY_FOOTER_F "\
    func_0x%llx(cpu,0)\n"

#define PY_FOOTER_N "\
    %s(cpu,0)\n"

#define PY_FOOTER_3 "\
    # Insert code here ...\n\
    return %i\n\
\n\
if __name__==\"__main__\":\n\
    sys.exit(main())\n\
\n"

void Lang_Py_x64::PrintMainClose(Code *c,char *name) {
    //printf(PY_FOOTER_2,c->ep);
    PrintF(PY_FOOTER_2);
    if (name != NULL) {
        PrintF(PY_FOOTER_N,name);
    }
    else {
        PrintF(PY_FOOTER_F,c->ep);
    }
    PrintF(PY_FOOTER_3,ERR_CODE_OK);
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
        PrintF("    cpu.add_mem(0x%llx,%s)\n",sm->addr,buffer);
        free(buffer);
    }
    else {
        PrintF("    cpu.add_zero_mem(0x%llx,%i)\n",sm->addr,sm->size);
    }
}

#define PY_FUNC_HEADER_NAME "\
@with_goto\n\
def %s(cpu,raddr):\n\
"

void Lang_Py_x64::PrintFuncHeaderName(Code *c,int num,char *name) {
    PrintF(PY_FUNC_HEADER_NAME,name,c->subcodes[num].first);
}

#define PY_FUNC_HEADER_ADDR "\
@with_goto\n\
def func_0x%llx(cpu,raddr):\n\
"

void Lang_Py_x64::PrintFuncHeaderAddr(Code *c,int num) {
    PrintF(PY_FUNC_HEADER_ADDR,c->subcodes[num].first);
}

/*
#define PY_FUNC_FOOTER "\
    label .label_return\n\
    return\n\
\n"
*/

void Lang_Py_x64::PrintAnonJmpVar(void) {
    // None
}

void Lang_Py_x64::PrintAnonJmpCall(uint64_t addr,char *name) {
    PrintF("    if anon == 0x%llx:\n",addr);
    PrintF("        goto %s\n",name);
}

void Lang_Py_x64::PrintAnonJmpEnd(void) {
    PrintF("    if anon == raddr:\n");
    PrintF("        return\n");
    PrintF("    cpu.panic(\"JMP\",\"0x%%llx\",raddr)\n");
}

void Lang_Py_x64::PrintFuncFooter(Code *c,int num) {
    //PrintF(PY_FUNC_FOOTER);
}

void Lang_Py_x64::PrintSubCodeSep(void) {
    PrintF("    # --------------------------------------------------------------\n");
}

void Lang_Py_x64::PrintLoadError(const char *code,int num,const char *msg) {
    PrintF("    cpu.errors[\"%s\"] = [%i,\"%s\"]\n",code,num,msg);
}

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
    return ("%s(cpu,0x%llx)");
}

const char *Lang_Py_x64::E_FUNC_ADDR(void) {
    return ("func_0x%llx(cpu,0x%llx)");
}

/*
const char *Lang_Py_x64::E_RETURN(void) {
    return ("goto .label_return");
}
*/

const char *Lang_Py_x64::E_JMP_FROM_IAT(void) {
    return ("cpu.jmp_from_iat(\"%s\",\"%s\")");
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

const char *Lang_Py_x64::F_SET_DF(void) {
    return ("cpu.flag_d(");
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

const char *Lang_Py_x64::F_GET_DF(void) {
    return ("cpu.get_flag_d(");
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

const char *Lang_Py_x64::F_SDIV(void) {
    return ("cpu.sdiv(");
}

const char *Lang_Py_x64::F_UDIV(void) {
    return ("cpu.udiv(");
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
/*
const char *Lang_Py_x64::F_POW(void) {
    return ("cpu.pow(");
}
*/
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
    return (ANON_CALL "(cpu,");
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

const char *Lang_Py_x64::E_TMP(void) {
    return ("cpu.tmp");
}

const char *Lang_Py_x64::E_TMP2(void) {
    return ("cpu.tmp2");
}

const char *Lang_Py_x64::F_MASK(void) {
    return ("cpu.mask(");
}

const char *Lang_Py_x64::F_UTOD(void) {
    return ("cpu.utod(");
}

const char *Lang_Py_x64::F_DTOU(void) {
     return ("cpu.dtou(");
}

const char *Lang_Py_x64::F_UTOF(void) {
    return ("cpu.utof(");
}

const char *Lang_Py_x64::F_FTOU(void) {
     return ("cpu.ftou(");
}

const char *Lang_Py_x64::F_LTOU(void) {
     return ("cpu.ltou(");
}

const char *Lang_Py_x64::F_UTOL(void) {
    return ("cpu.utol(");
}

const char *Lang_Py_x64::E_EFLAGS(void) {
     return ("cpu.eflags.r32");
}

const char *Lang_Py_x64::E_ELSE(void) {
     return ("else:");
}

const char *Lang_Py_x64::E_PANIC_JMP_INDEX(void) {
    return ("cpu.panic(cpu,\"JMP\",\"%s\")");
}