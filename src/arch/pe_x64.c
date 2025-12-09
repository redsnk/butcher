
// https://www.capstone-engine.org/documentation.html
// https://www.capstone-engine.org/lang_c.html

#include "pe_x64.h"
#include "../files/pe.h"
#include <capstone/capstone.h>

char *Cut_pe_x64(char *file_name,uint64_t address,char *func_name) {
struct _PE *pe;
csh handle;
cs_insn *insn;
size_t count;
uint64_t addr;
int lexit;

    pe = GetPE(file_name);
    if (pe != NULL) {
        if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) == CS_ERR_OK) {
            cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
            addr = address;
            lexit = false;
            while (!lexit) {
                uint8_t *m = GetMemoryPE(pe,addr,0x400);
                if (m != NULL) {
                    count = cs_disasm(handle, m, 0x400, addr, 0, &insn);
                    if (count) {
                        for (int n = 0; (!lexit) && (n < count); n++) {
                            printf("0x%"PRIx64":\t%s\t\t%s\n", insn[n].address, insn[n].mnemonic,insn[n].op_str);
                            if (insn[n].detail->groups_count > 0) {
                                for (int i=0;i <  insn[n].detail->groups_count;i++) {
                                    if (insn[n].detail->groups[i] == X86_GRP_RET) {
                                        lexit = true;
                                        break;
                                    }
                                }
                            }
                            cs_detail *detail = insn->detail;
                            if (detail->x86.op_count) {
                                
                            }
                        }
                        cs_free(insn, count);
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
            
        }
        FreePE(pe);
    }  
}

