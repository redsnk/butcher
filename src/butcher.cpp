// https://www.capstone-engine.org/documentation.html
// https://www.capstone-engine.org/lang_c.html

#include "butcher.hpp"

Code::Code(uint64_t a) {
    next_id = SUBCODE_TOP+1;
    ep = a;
    subcod_count = 0;
    submem_count = 0;
}

void Code::NewSubCode (struct _subcode *sc) {
    sc->id = next_id++;
    sc->name = NULL;
}

void Code::AddSubcode (struct _subcode *sc) {
    if (sc->parent != SUBCODE_TOP) {
        for (int n=0;n<subcod_count;n++) {
            if ((sc->parent == subcodes[n].id) || (sc->parent == subcodes[n].parent)) {
                // Parent or same parent
                if((sc->first >= subcodes[n].first) && (sc->last <= subcodes[n].last)) {
                    // Subcode included in the old one, discard it
                    return;
                }
                if((subcodes[n].first >= sc->first) && (subcodes[n].last <= sc->last)) {
                    // Old subcode included in this one, replace it
                    cs_free(subcodes[n].insn, subcodes[n].count);
                    subcodes[n] = *sc;
                    return;
                }
            }
        }
    }
    if (!subcod_count) {
        subcodes = (struct _subcode *) malloc(sizeof(struct _subcode));
    } else {
        subcodes = (struct _subcode *) realloc(subcodes, sizeof(struct _subcode)*(subcod_count+1));
    }
    //sc->id = next_id++;
    //sc->parent = parent;
    subcodes[subcod_count++] = *sc;
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

int Code::HasAddr (uint64_t addr,int parent) {
    if (parent != SUBCODE_TOP) {
        // jmp
        for (int n=0;n<subcod_count;n++) {
            if ((parent == subcodes[n].id) || (parent == subcodes[n].parent)) {
                if ((addr >= subcodes[n].first) && (addr <= subcodes[n].last)) {
                    return (true);
                }
            }
        }
    }
    else {
        // call
        for (int n=0;n<subcod_count;n++) {
            if ((subcodes[n].parent == SUBCODE_TOP) && (addr == subcodes[n].first)) {
                return (true);
            }
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

char *Code::GetFunctionName (uint64_t addr) {
int n;

    for (n=0;n<subcod_count;n++) {
        if ((subcodes[n].parent == SUBCODE_TOP) && (subcodes[n].first == addr) && (subcodes[n].name != NULL)) {
            return (strdup(subcodes[n].name));
        }
    }
    return (NULL);
}

Code::~Code() {
int n;

    if (subcod_count > 0) {
        for (n=0;n<subcod_count;n++) {
            cs_free(subcodes[n].insn, subcodes[n].count);
            if (subcodes[n].name != NULL) {
                free(subcodes[n].name);
            }
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

Code *Butcher::GetCode(Code *c,uint64_t address,char *name,int parent) {
int lexit,lend;
struct _subcode sc;
int n;
//std::set<uint64_t> calls;
struct _call *calls;
int ncalls = 0;
std::set<uint64_t> jmps;
uint64_t addr,read;
int max_subcode = INIT_MEM_GETCODE;

    if ((address == 0x18000af00) && (parent == SUBCODE_TOP)) {
        //printf("// test\n");
    }
    if (c == NULL) {
        c = new Code(address);
    }
    if (c->HasAddr(address,parent)) {
        printf("%s *** GetCode repeated 0x%llx, exit ...\n",lang->COMM,address);
        return (c);
    }
    c->NewSubCode(&sc);
    sc.name = name;
    sc.parent = parent;
    sc.first = address;
    printf("%s *** GetCode 0x%llx (id=%i,parent=%i)\n",lang->COMM,sc.first,sc.id,sc.parent);
    lexit = false;
    while (!lexit) {
        uint8_t *m = arch->GetMemory(sc.first,max_subcode,&read);
        // TODO: read < max_subcode
        if ((m != NULL) && (read == max_subcode)) {
            sc.count = cs_disasm(handle, m, max_subcode, sc.first, 0, &sc.insn);
            if (sc.count) {
                for (n = 0; n < sc.count; n++) {
                    lend = false;
                    printf("// 0x%llx:\t%s\t\t%s\n", sc.insn[n].address, sc.insn[n].mnemonic,sc.insn[n].op_str);
                    if (IsCall(sc.insn[n],&addr)) {
                        // New subcode 
                        //printf("// *** Add call 0x%llx\n",addr);
                        //calls.insert(addr);
                        if (!ncalls) {
                            calls = (struct _call *) malloc(sizeof(struct _call));
                        }
                        else {
                            calls = (struct _call *) realloc(calls,sizeof(struct _call)*(ncalls+1));
                        }
                        calls[ncalls].addr = addr;
                        calls[ncalls].name = NULL;
                        if (arch->IsSymbolFunction(addr,&calls[ncalls].name)) {
                            printf("%s *** Add call 0x%llx(%s)\n",lang->COMM,addr,calls[ncalls].name);
                        }
                        else {
                            printf("%s *** Add call 0x%llx\n",lang->COMM,addr);
                        }
                        ncalls++;
                    }
                    else if (IsJcc(sc.insn[n],&addr)) {
                        //printf("jcc 0x%llx\n",addr);
                        printf("%s *** Add jcc 0x%llx\n",lang->COMM,addr);
                        jmps.insert(addr);
                        c->labels.insert(addr);
                    }
                    else if (IsJmp(sc.insn[n],&addr)) {
                        //printf("jmp 0x%llx\n",addr);
                        if (addr) {
                            printf("%s *** Add jmp 0x%llx\n",lang->COMM,addr);
                            jmps.insert(addr);
                            c->labels.insert(addr);
                        }
                        lend = true;
                    }
                    else if (IsRet(sc.insn[n]) || IsInt(sc.insn[n],&addr) || IsEnd(sc.insn,n,sc.count)) {
                        lend = true;
                    }
                    if (lend) {
                        // End subcode
                        printf("%s *** End of subcode 0x%llx\n",lang->COMM,sc.first);
                        sc.last = sc.insn[n].address;
                        lexit = true;
                        break;
                    }
                }
                if (max_subcode > MAX_MEM_GETCODE) {
                    // No more code, maximun reached
                    sc.last = sc.insn[n].address;
                    lexit = true;
                }
                if (!lexit) {
                    // We need more code
                    cs_free(sc.insn, sc.count);
                    max_subcode += STEP_MEM_GETCODE;
                } else {
                    // Done
                    printf("%s *** Add subcode 0x%llx %li (parent=%i)\n",lang->COMM,sc.first,sc.last-sc.first,sc.parent);
                    c->AddSubcode(&sc);
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
    for (uint64_t a : jmps) {
        if ((a < sc.first) || (a > sc.last)) {
            // Jmp outside the subcode, explore child subcode
            c = GetCode(c,a,NULL,(parent==SUBCODE_TOP)?sc.id:parent);
        }
    }
    /*
    for (uint64_t a : calls) {
        // Explore new addresses for calls
        c = GetCode(c,a,NULL,SUBCODE_TOP);
    }
    */
    for (int n=0;n<ncalls;n++) {
        // Explore new addresses for calls
        c = GetCode(c,calls[n].addr,calls[n].name,SUBCODE_TOP);
    }
    if (ncalls) {
        free (calls);
    }
    return (c);
}

void Butcher::Cut(char *file_name,uint64_t address) {
    if (arch->OpenFile(file_name)) {
        if (Cs_open() == CS_ERR_OK) {
            cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
            /*
            char lib[256];
            char func[256];
            IsImportFunction(0x115378,lib,func);
            IsSymbolFunction(0xbf890,func);
            IsSymbolObject(0x11f5f8,func);
            */
            Code *c = GetCode(NULL,address,NULL,SUBCODE_TOP);
            //c->Print();
            PrintCode(c);
            delete c;
        }
        arch->CloseFile();
    }
}

Butcher::Butcher(Archive *a,Language *l) {
    lang = l;
    arch = a;
}

Butcher::~Butcher() {
    delete lang;
    delete arch;
}
