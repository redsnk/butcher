#include "language.hpp"

#define MAX_STR_OP     (1024)

const char *Language::ptr(cs_x86_op op) {
    switch (op.size) {
        case 1:
            return("byte");
        case 2:
            return("word");
        case 4:
            return("dword");
        case 8:
            return("qword");
    }
    return ("ptr_error");
}

char *Language::get_op_str(csh handle,cs_x86_op op,int sign) {
char buffer[MAX_STR_OP];
char *str;

    switch (op.type) {
        case X86_OP_REG:
            if (!sign) {
                return (reg_name(handle,op.reg));
            }
            else {
                return (s_reg_name(handle,op.reg));
            }
        case X86_OP_MEM:
            str = mem_str(handle,op);
            sprintf(buffer,(sign)?E_S_GET_MEM:E_GET_MEM,ptr(op),str);
            free(str);
            return (strdup(buffer));
        case X86_OP_IMM:
            sprintf(buffer,"0x%llx",op.imm);
            return (strdup(buffer));
    }
    return(NULL);
}