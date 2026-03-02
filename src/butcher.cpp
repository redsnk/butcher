// https://www.capstone-engine.org/documentation.html
// https://www.capstone-engine.org/lang_c.html

#include "butcher.hpp"

int Butcher::IsGroup (cs_insn *insn, int group) {
    if (insn->detail->groups_count > 0) {
        for (int i=0;i <  insn->detail->groups_count;i++) {
            if (insn->detail->groups[i] == group) {
                return (true);
            }
        }
    }
    return (false);
}

int Butcher::Excluded(uint64_t addr) {
    if (std::find(ex.begin(), ex.end(), addr) != ex.end()) {
        return (true);
    }
    return (false);
}

int Butcher::IsNamedFunction (uint64_t addr, char **func) {
    if (named.count(addr)) {
        *func = strdup(named.at(addr).c_str());
        return (true);
    }
    return (false);
}

Code *Butcher::GetCode(Code *c,uint64_t address,char *name,int parent) {
int lexit,lend;
struct _subcode sc;
int n,nn,count;
//std::set<uint64_t> calls;
struct _call *calls;
int ncalls = 0;
std::set<uint64_t> jmps;
uint64_t addr,read,*addr_list;
int max_subcode = INIT_MEM_GETCODE;
uint8_t *mem;

    if ((address == 0x18000af00) && (parent == SUBCODE_TOP)) {
        //printf("// test\n");
    }
    if (c == NULL) {
        c = new Code(address);
    }
    if (c->HasAddr(address,parent)) {
        if (ltraces) printf("%s *** GetCode repeated 0x%llx, exit ...\n",lang->COMM,address);
        return (c);
    }
    c->NewSubCode(&sc);
    sc.name = name;
    sc.parent = parent;
    sc.first = address;
    if (ltraces) printf("%s *** GetCode 0x%llx (id=%i,parent=%i)\n",lang->COMM,sc.first,sc.id,sc.parent);
    lexit = false;
    while (!lexit) {
        uint8_t *m = arch->GetMemory(sc.first,max_subcode,&read);
        // TODO: read < max_subcode
        if ((m != NULL) && (read == max_subcode)) {
            sc.count = cs_disasm(handle, m, max_subcode, sc.first, 0, &sc.insn);
            if (sc.count) {
                for (n = 0; n < sc.count; n++) {
                    lend = false;
                    if (ltraces) printf("%s 0x%llx:\t%s\t\t%s\n",lang->COMM, sc.insn[n].address, sc.insn[n].mnemonic,sc.insn[n].op_str);
                    if (!loadm && IsSubMem(&sc.insn[n],&addr,&mem,&count)) {
                        // New submem
                        if (ltraces) printf("%s *** Add submem 0x%llx(%li)\n",lang->COMM,addr,count);
                        c->AddSubMem(addr,mem,count);
                        free(mem);
                    }
                    if (IsCall(&sc.insn[n],&addr)) {
                        if (!Excluded(addr)) {
                            // New subcode 
                            if (!ncalls) {
                                calls = (struct _call *) malloc(sizeof(struct _call));
                            }
                            else {
                                calls = (struct _call *) realloc(calls,sizeof(struct _call)*(ncalls+1));
                            }
                            calls[ncalls].addr = addr;
                            calls[ncalls].name = NULL;
                            if (IsNamedFunction(addr,&calls[ncalls].name) || arch->IsSymbolFunction(addr,&calls[ncalls].name)) {
                                if (ltraces) printf("%s *** Add call 0x%llx(%s)\n",lang->COMM,addr,calls[ncalls].name);
                            }
                            else {
                                if (ltraces) printf("%s *** Add call 0x%llx\n",lang->COMM,addr);
                            }
                            ncalls++;
                        }
                    }
                    else if (IsJcc(&sc.insn[n],&addr)) {
                        //printf("jcc 0x%llx\n",addr);
                        if (ltraces) printf("%s *** Add jcc 0x%llx\n",lang->COMM,addr);
                        jmps.insert(addr);
                        c->labels.insert(addr);
                    }
                    else if(IsJmp(&sc.insn[n],&addr_list,&count)) {
                        for (nn=0;nn<count;nn++) {
                            if (addr_list[nn] != UNDEF_ADDR_JMP) {
                                jmps.insert(addr_list[nn]);
                                c->labels.insert(addr_list[nn]);
                            }
                        }
                        free (addr_list);
                        lend = true;
                    }
                    /*
                    else if (IsJmp(&sc.insn[n],&addr)) {
                        //printf("jmp 0x%llx\n",addr);
                        if (addr) {
                            printf("%s *** Add jmp 0x%llx\n",lang->COMM,addr);
                            jmps.insert(addr);
                            c->labels.insert(addr);
                        }
                        lend = true;
                    }
                    */
                    else if (IsRet(&sc.insn[n]) || IsInt(&sc.insn[n],&addr) || IsEnd(sc.insn,n,sc.count) ||IsJmpIAT(&sc.insn[n]) ) {
                        lend = true;
                    }
                    if (lend) {
                        // End subcode
                        if (ltraces) printf("%s *** End of subcode 0x%llx\n",lang->COMM,sc.first);
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
                    if (ltraces) printf("%s *** Add subcode 0x%llx %li (parent=%i)\n",lang->COMM,sc.first,sc.last-sc.first,sc.parent);
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
    ltraces = false;
    lasm = false;
    loadm = false;
}

Butcher::~Butcher() {
    delete lang;
    delete arch;
}
