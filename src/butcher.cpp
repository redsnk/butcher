// https://www.capstone-engine.org/documentation.html
// https://www.capstone-engine.org/lang_c.html

#include "butcher.hpp"

Code::Code(uint64_t a) {
    ep = a;
    subcod_count = 0;
    submem_count = 0;
}

void Code::AddSubcode (struct _subcode sc) {
    for (int n=0;n<subcod_count;n++) {
        if((sc.first < subcodes[n].first) && (subcodes[n].first <= sc.last)) {
            // Same code but sc bigger
            cs_free(subcodes[n].insn, subcodes[n].count);
            subcodes[n] = sc;
            return;
        }
    }
    if (!subcod_count) {
        subcodes = (struct _subcode *) malloc(sizeof(struct _subcode));
    } else {
        subcodes = (struct _subcode *) realloc(subcodes, sizeof(struct _subcode)*(subcod_count+1));
    }
    subcodes[subcod_count++] = sc;
}

void Code::AddSubMem (uint64_t address,uint8_t *mem,uint64_t size) {
    for(int n=0;n<submem_count;n++) {
        if ((submems[n].addr >= address) && ((submems[n].addr+submems[n].size) <= (address+size))) {
            return;
        }
    }
    if (!submem_count) {
        submems = (struct _submem *) malloc(sizeof(struct _submem));
    } else {
        submems = (struct _submem *) realloc(submems, sizeof(struct _submem)*(submem_count+1));
    }
    submems[submem_count].mem = (uint8_t *) malloc(size);
    memcpy(submems[submem_count].mem,mem,size);
    submems[submem_count].size = size;
    submems[submem_count].addr = address;
    submem_count++;
}

int Code::HasAddr (uint64_t addr) {
    for (int n=0;n<subcod_count;n++) {
        if ((addr >= subcodes[n].first) && (addr <= subcodes[n].last)) {
            return (true);
        }
    }
    return (false);
}

void Code::Print (void) {
int n,i;

    for (n=0;n<subcod_count;n++) {
        printf("==== SC(%i) ====\n",n);
        for (i=0;i<subcodes[n].count;i++) {
            printf("0x%llx:\t%s\t\t%s\n", subcodes[n].insn[i].address, subcodes[n].insn[i].mnemonic,subcodes[n].insn[i].op_str);
            if (subcodes[n].insn[i].address == subcodes[n].last) {
                break;
            }
        }
    }
}

Code::~Code() {
int n;

    if (subcod_count > 0) {
        for (n=0;n<subcod_count;n++) {
            cs_free(subcodes[n].insn, subcodes[n].count);
        }
        free(subcodes);
    }
    if (submem_count > 0) {
        for (n=0;n<submem_count;n++) {
            free(submems[n].mem);
        }
        free(submems);
    }
}

// ----------------------

int Butcher::IsGroup (cs_insn insn, int group) {
    if (insn.detail->groups_count > 0) {
        for (int i=0;i <  insn.detail->groups_count;i++) {
            if (insn.detail->groups[i] == group) {
                return (true);
            }
        }
    }
    return (false);
}

Code *Butcher::GetCode(Code *c,uint64_t address) {
int lexit;
struct _subcode sc;
int n;
std::list<uint64_t> calls;
std::list<uint64_t> jmps;
uint64_t addr;
int max_subcode = INIT_MEM_GETCODE;

    if (c == NULL) {
        c = new Code(address);
    }
    if (c->HasAddr(address)) {
        return (c);
    }
    sc.first = address;
    lexit = false;
    while (!lexit) {
        uint8_t *m = GetMemory(sc.first,max_subcode);
        if (m != NULL) {
            sc.count = cs_disasm(handle, m, max_subcode, sc.first, 0, &sc.insn);
            if (sc.count) {
                for (n = 0; n < sc.count; n++) {
                    //printf("0x%llx:\t%s\t\t%s\n", sc.insn[n].address, sc.insn[n].mnemonic,sc.insn[n].op_str);
                    if (IsCall(sc.insn[n],&addr)) {
                        // Explore later 
                        calls.push_back(addr);
                    }
                    if (IsJmp(sc.insn[n],&addr)) {
                        // TODO: check out of code jmps
                        jmps.push_back(addr);
                        c->labels.push_back(addr);
                    }
                    if (IsRet(sc.insn[n])) {
                        // End subcode
                        sc.last = sc.insn[n].address;
                        lexit = true;
                        break;
                    }
                    if (IsInt(sc.insn[n],&addr)) {
                        // End subcode
                        sc.last = sc.insn[n].address;
                        lexit = true;
                        break;
                    }
                }
                if (!lexit) {
                    // not enough space, try wtih more
                    cs_free(sc.insn, sc.count);
                    max_subcode += STEP_MEM_GETCODE;
                } else {
                    // done
                    c->AddSubcode(sc);
                }
                //sc.first = sc.last + sc.insn[n-1].size;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    for (uint64_t a : calls) {
        // Explore new addresses
        c = GetCode(c,a);
    }
    return (c);
}

void Butcher::Cut(char *file_name,uint64_t address) {
//cs_insn *insn;
//size_t count;
//uint64_t addr;
//int lexit;

    if (OpenFile(file_name)) {
        if (Cs_open() == CS_ERR_OK) {
            cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
            Code *c = GetCode(NULL,address);
            //c->Print();
            PrintCodeC(c);
            delete c;
        }
        CloseFile();
    }
}
