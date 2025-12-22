#include "pe_x64.hpp"
#include "../files/pe.hpp"
#include <capstone/capstone.h>

int Pe_x64::OpenFile(char *file_name) {
    pe = GetPE(file_name);
    return(pe != NULL);
}

void Pe_x64::CloseFile(void) {
    FreePE(pe);
}

cs_err Pe_x64::Cs_open(void) {
    return(cs_open(CS_ARCH_X86, CS_MODE_64, &handle));
}

uint8_t *Pe_x64::GetMemory(uint64_t addr,uint64_t size) {
    return(GetMemoryPE(pe,addr,size));
}

int Pe_x64::IsRet(cs_insn insn) {
    return(IsGroup(insn,X86_GRP_RET));
}

int Pe_x64::IsCall(cs_insn insn, uint64_t *addr) {
    if (IsGroup(insn,X86_GRP_CALL)) {
        if (insn.detail->x86.op_count == 1) {
            if (insn.detail->x86.operands[0].type == X86_OP_IMM) {
                *addr = insn.detail->x86.operands[0].imm;
                return (true);
            }
        }
    }
    return (false);
}

int Pe_x64::IsInt(cs_insn insn, uint64_t *num) {
    return(IsGroup(insn,X86_GRP_INT));
}

int Pe_x64::IsJmp(cs_insn insn, uint64_t *addr) {
    if (IsGroup(insn,X86_GRP_JUMP)) {
        if (insn.detail->x86.op_count == 1) {
            if (insn.detail->x86.operands[0].type == X86_OP_IMM) {
                *addr = insn.detail->x86.operands[0].imm;
                return (true);
            }
        }
    }
    return (false);
}

int Pe_x64::IsImport(cs_insn insn, char **name) {
    return (false);
}

#define C_HEADER "\
#include \"x64.h\"\n\
\n"

#define C_FOOTER "\
int main (int argc, char **argv) {\n\
struct _cpu cpu;\n\
\n\
    init(&cpu);\n\
    func_0x%llx(&cpu);\n\
    return (0);\n\
}\n\
\n"

#define C_FUNC_HEADER "\
void func_0x%llx(struct _cpu *cpu) {\n\
"

#define C_FUNC_FOOTER "\
}\n\
\n"

/*
uint8_t *Pe_x64::PrintInst(cs_insn *insn) {
    switch (insn->id) {
        case X86_INS_PUSH:
            printf("    push(cpu,\"%s\");",insn->op_str);
            break;
        case X86_INS_LEA:
            if ((insn->detail->x86.operands[0].type == X86_OP_REG) || (insn->detail->x86.operands[1].type == X86_OP_MEM && handle,insn->detail->x86.operands[1].mem.index == X86_REG_INVALID)) {
                printf("    lea(cpu,\"%s\",\"%s\",0x%llx);",    cs_reg_name(handle,insn->detail->x86.operands[0].reg),
                                                                cs_reg_name(handle,insn->detail->x86.operands[1].mem.base),
                                                                insn->detail->x86.operands[1].mem.disp);
                break;
            }
            printf("    error: X86_INS_LEA");
            break;
        case X86_INS_SUB:
            break;
        default:
            printf("    printf(\"Error: PrintIns\\n\");exit(1);");
            break;
    }
    return (NULL);
}
*/

const char *reg_name(csh handle,int id_reg) {
    if (id_reg == X86_REG_INVALID) {
        return ("");
    }
    return(cs_reg_name(handle,id_reg));
}

int Pe_x64::PrintExtra(struct _subcode *sc,int num) {
    return (num);
}

int Pe_x64::PrintInst(struct _subcode *sc,int num) {
int n;
char subname[1024];
char params[1024];
char buffer[1024];
cs_insn *insn;

    int ret = PrintExtra(sc,num);
    if (ret > num) {
        return (ret);
    }
    subname[0] = 0;
    params[0] = 0;
    insn = &sc->insn[num];
    for (n=0;n<insn->detail->x86.op_count;n++) {
        switch (insn->detail->x86.operands[n].type) {
            case X86_OP_REG:
                strcat(subname,"r");
                sprintf(buffer,"\"%s\",",reg_name(handle,insn->detail->x86.operands[n].reg));
                strcat(params,buffer);
                break;
            case X86_OP_IMM:
                strcat(subname,"i");
                sprintf(buffer,"0x%llx,",insn->detail->x86.operands[n].imm);
                strcat(params,buffer);
                break;
            case X86_OP_MEM:
                strcat(subname,"m");
                sprintf(buffer,"\"%s\",\"%s\",%i,0x%llx,",reg_name(handle,insn->detail->x86.operands[n].mem.base),
                                                reg_name(handle,insn->detail->x86.operands[n].mem.index),
                                                insn->detail->x86.operands[n].mem.scale,
                                                insn->detail->x86.operands[n].mem.disp);
                strcat(params,buffer);
                break;
            default:
                strcat(subname,"e");
                break;
        }
    }
    int l = strlen(params);
    if (l) {
        params[l-1] = 0;
    }
    if (insn->detail->x86.op_count) {
        printf("    %s_%s(cpu,%s);",insn->mnemonic,subname,params);
    } else {
        printf("    %s(cpu);",insn->mnemonic);
    }
    return (num+1);
}

void Pe_x64::PrintSubCodeC(Code *c,int num) {
struct _subcode *sc;

    sc = &c->subcodes[num];
    printf(C_FUNC_HEADER,sc->first);
    for (int n=0;n<sc->count;) {
        if (sc->insn[n].address > sc->last) {
            break;
        }
        if (std::find(c->labels.begin(), c->labels.end(), sc->insn[n].address) != c->labels.end()) {
            printf("label_0x%llx:\n",sc->insn[n].address);
        }
        n = PrintInst(sc,n);
        printf("    //0x%llx:\t%s\t\t%s\n", sc->insn[n].address, sc->insn[n].mnemonic,sc->insn[n].op_str);
    }
    printf(C_FUNC_FOOTER);
}

void Pe_x64::PrintCodeC(Code *c) {
    printf(C_HEADER);
    for (int n=0;n<c->count;n++) {
        PrintSubCodeC(c,n);
    }
    printf(C_FOOTER,c->ep);
}
