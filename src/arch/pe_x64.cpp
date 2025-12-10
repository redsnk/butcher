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

