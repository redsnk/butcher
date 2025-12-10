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

#define C_HEADER "\
#include <stddef.h>\n\
#include <stdlib.h>\n\
#include <stdio.h>\n\
#include <stdint.h>\n\
\n\
#define MAX_STACK 1024\n\
\n\
struct _regs {\n\
    uint64_t rax,rbx,rcx,rdx,r8,r9,r10,r11,r12,r13,r14,r15,rdi,rsi,rbp,rsp;\n\
    uint64_t stack[MAX_STACK];\n\
\n\
};\n\
\n"

#define C_FOOTER "\
int main (int argc, char **argv) {\n\
struct _regs r;\n\
\n\
    r.rsp = (uint64_t)(r.stack+(MAX_STACK/2));\n\
    func_0x%x(&r);\n\
    return (0);\n\
}\n\
\n"

#define C_FUNC_HEADER "\
void func_0x%x(struct _regs *r) {\n\
"

#define C_FUNC_FOOTER "\
}\n\
\n"

uint8_t *Pe_x64::PrintSubCodeC(struct _subcode *sc) {
    printf(C_FUNC_HEADER,sc->first);
    for (int n=0;n<sc->count;n++) {
        if (sc->insn[n].address > sc->last) {
            break;
        }
        printf("    //0x%x:\t%s\t\t%s\n", sc->insn[n].address, sc->insn[n].mnemonic,sc->insn[n].op_str);
    }
    printf(C_FUNC_FOOTER);
    return (NULL);
}

uint8_t *Pe_x64::PrintCodeC(Code *c) {
    printf(C_HEADER);
    for (int n=0;n<c->count;n++) {
        PrintSubCodeC(&c->subcodes[n]);
    }
    printf(C_FOOTER,c->ep);
    return (NULL);
}

