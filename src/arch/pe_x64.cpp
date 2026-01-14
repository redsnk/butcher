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

uint8_t *Pe_x64::GetMemory(uint64_t addr,uint64_t size, uint64_t *read) {
    return(GetMemoryPE(pe,addr,size,read));
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

#define C_FUNC_PROTO "\
void func_0x%llx(struct _cpu *cpu);\n"

#define C_FOOTER_1 "\
int main (int argc, char **argv) {\n\
struct _cpu cpu;\n\
\n\
    init(&cpu);\n\
"

#define C_FOOTER_2 "\
    func_0x%llx(&cpu);\n\
    end(&cpu);\n\
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

int Pe_x64::PrintExtra(Code *c,struct _subcode *sc,int num) {
cs_insn *insn;
uint64_t read;

    insn = &sc->insn[num];
    switch (insn->id) {
        case X86_INS_RET:
            printf("    return;");
            printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
            num++;
            break;
        case X86_INS_JMP:
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                printf("    goto label_0x%llx;",insn->detail->x86.operands[0].imm);
                printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
                num++;
            }
            break;
        case X86_INS_JE:
            printf("    if (flag_z(cpu)) goto label_0x%llx;",insn->detail->x86.operands[0].imm);
            printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
            num++;
            break;
        case X86_INS_JNE:
            printf("    if (!flag_z(cpu)) goto label_0x%llx;",insn->detail->x86.operands[0].imm);
            printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
            num++;
            break;
        case X86_INS_JA:
            printf("    if (!flag_c(cpu) && !flag_z(cpu)) goto label_0x%llx;",insn->detail->x86.operands[0].imm);
            printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
            num++;
            break;
        case X86_INS_JAE:
            printf("    if (!flag_c(cpu)) goto label_0x%llx;",insn->detail->x86.operands[0].imm);
            printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
            num++;
            break;
        case X86_INS_MOV:
            if (insn->detail->x86.operands[1].type == X86_OP_MEM) {
                if (insn->detail->x86.operands[1].mem.base == X86_REG_RIP) {
                    // mov rax, qword ptr [rip + 0x1dc97]
                    uint64_t addr = insn->address + insn->size + insn->detail->x86.operands[1].mem.disp;
                    uint8_t *mem = GetMemoryPE(pe,addr,8,&read);
                    if ((mem != NULL) && (read == 8)) {
                        c->AddSubMem(addr,mem,8);
                        free(mem);
                    }
                    //printf("    cpu->%s.r64 = qword_ptr(cpu,0x%llx);",reg_name(handle,insn->detail->x86.operands[0].reg),addr);
                    switch (insn->detail->x86.operands[0].size) {
                        case 1:
                            printf("    *(get_reg_8(cpu,\"%s\")) = byte_ptr(cpu,0x%llx);",reg_name(handle,insn->detail->x86.operands[0].reg),addr);
                            break;
                        case 2:
                            printf("    *(get_reg_16(cpu,\"%s\")) = word_ptr(cpu,0x%llx);",reg_name(handle,insn->detail->x86.operands[0].reg),addr);
                            break;
                        case 4:
                            printf("    *(get_reg_32(cpu,\"%s\")) = dword_ptr(cpu,0x%llx);",reg_name(handle,insn->detail->x86.operands[0].reg),addr);
                            break;
                        case 8:
                            printf("    *(get_reg_64(cpu,\"%s\")) = qword_ptr(cpu,0x%llx);",reg_name(handle,insn->detail->x86.operands[0].reg),addr);
                            break;
                        default:
                            printf("*** PrintExtra error bits\n");
                            break;
                    }
                    printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
                    num++;
                }
            }
            break;
        case X86_INS_CALL:
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                printf("    func_0x%llx(cpu);",insn->detail->x86.operands[0].imm);
                printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
                num++;
            } else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if (insn->detail->x86.operands[0].mem.base == X86_REG_RIP) {
                    uint64_t addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;
                    /*
                    uint64_t *n = (uint64_t *) GetMemoryPE(pe,addr,8);
                    printf("value = 0x%llx\n",*n);
                    */
                    struct _import_name in;
                    if (GetImportFunction (pe,addr,&in)) {
                        printf("    call_from_iat(\"%s\",\"%s\");",in.lib_name,in.func_name);
                        printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
                        num++;
                    }

                }             
            }
            break;
    }
    return (num);
}

int Pe_x64::PrintInst(Code *c,struct _subcode *sc,int num) {
int n;
char subname[1024];
char params[1024];
char buffer[1024];
cs_insn *insn;

    int ret = PrintExtra(c,sc,num);
    if (ret > num) {
        // Done, next instruction
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
        //printf("    %s_%s(cpu,%s);",insn->mnemonic,subname,params);
        printf("    op_%s(cpu,\"%s\",%s);",subname,insn->mnemonic,params);
    } else {
        //printf("    %s(cpu);",insn->mnemonic);
        printf("    op(cpu,\"%s\");",insn->mnemonic);
    }
    printf("    //0x%llx:\t%s\t\t%s\n", insn->address, insn->mnemonic,insn->op_str);
    return (num+1);
}

void Pe_x64::PrintSubCodeC(Code *c,int num) {
struct _subcode *sc;
int id;

    id = c->subcodes[num].id;
    printf(C_FUNC_HEADER,c->subcodes[num].first);
    for (int m=0;m<c->subcod_count;m++) {
        sc = &c->subcodes[m];
        if ((sc->id == id) || (sc->parent == id)) {
            if (sc->parent != SUBCODE_TOP) {
                printf("    // --------------------------------------------------------------\n");
            }
            for (int n=0;n<sc->count;) {
                if (sc->insn[n].address > sc->last) {
                    break;
                }
                if (std::find(c->labels.begin(), c->labels.end(), sc->insn[n].address) != c->labels.end()) {
                    printf("label_0x%llx:\n",sc->insn[n].address);
                }
                n = PrintInst(c,sc,n);
                //printf("    //0x%llx:\t%s\t\t%s\n", sc->insn[n].address, sc->insn[n].mnemonic,sc->insn[n].op_str);
            }
        }
    }
    printf(C_FUNC_FOOTER);
}

void Pe_x64::PrintSubMemC(Code *c,int num) {
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

void Pe_x64::PrintCodeC(Code *c) {
    printf(C_HEADER);
    for (int n=0;n<c->subcod_count;n++) {
        printf(C_FUNC_PROTO,c->subcodes[n].first);
    }
    printf("\n");
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            PrintSubCodeC(c,n);
        }
    }
    printf(C_FOOTER_1);
    for (int n=0;n<c->submem_count;n++) {
        PrintSubMemC(c,n);
    }
    printf(C_FOOTER_2,c->ep);
}
