// https://www.capstone-engine.org/lang_c.html

#include "butcher.h"
#include <capstone/capstone.h>

 char *Cut(struct _PE *pe,uint64_t address) {
csh handle;
cs_insn *insn;
size_t count;

    uint8_t *m = GetMemoryPE(pe,address,0x400);
    if (m != NULL) {
        if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) == CS_ERR_OK) {
            cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
            count = cs_disasm(handle, m, 0x400, address, 0, &insn);
            if (count > 0) {
                size_t j;
                for (j = 0; j < count; j++) {
                    printf("0x%"PRIx64":\t%s\t\t%s\n", insn[j].address, insn[j].mnemonic,insn[j].op_str);
                }
                cs_free(insn, count);
            }
        }
        free(m);
    }
    return (NULL);
 }