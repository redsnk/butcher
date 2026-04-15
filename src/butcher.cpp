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
    else if (arch->IsSymbolFunction(addr,func)) {
        return (true);
    }
    *func = NULL;
    return (false);
}

Code *Butcher::Include(Code *c) {
std::set<uint64_t>::iterator itr;
char *func;

  for (itr = in.begin();itr != in.end(); itr++) {
    func = NULL;
    IsNamedFunction(*itr,&func);
    c = GetCode(c,*itr,func,SUBCODE_TOP);
  }
  return (c);
}

Code *Butcher::GetCode(Code *c,uint64_t address,char *name,int parent) {
int lexit,lend;
struct _subcode sc;
int n,nn,count,anon,bytes;
//std::set<uint64_t> calls;
struct _call *calls;
int ncalls = 0;
std::set<uint64_t> jmps;
uint64_t addr,read,*addr_list;
int max_subcode = INIT_MEM_GETCODE;
uint8_t *mem;
char *lib,*func;

    /*
    if (address == 0x44fc5cb) {
        ncalls = 0; // test
    }
    */
    if (c == NULL) {
        c = new Code(address);
    }
    if (c->HasAddr(address,parent)) {
        if (ltraces) printf("%s *** GetCode repeated 0x%llx, exit ...\n",lang->COMM(),address);
        return (c);
    }
    c->NewSubCode(&sc);
    sc.name = name;
    sc.parent = parent;
    sc.first = address;
    if (ltraces) printf("%s *** GetCode 0x%llx (id=%i,parent=%i)\n",lang->COMM(),sc.first,sc.id,sc.parent);
    lexit = false;
    while (!lexit) {
        uint8_t *m = arch->GetMemory(sc.first,max_subcode,&read);
        // TODO: read < max_subcode
        if ((m != NULL) && (read == max_subcode)) {
            sc.count = cs_disasm(handle, m, max_subcode, sc.first, 0, &sc.insn);
            if (sc.count) {
                for (n = 0; n < sc.count; n++) {
                    lend = false;
                    if (ltraces) printf("%s 0x%llx:\t%s\t\t%s\n",lang->COMM(), sc.insn[n].address, sc.insn[n].mnemonic,sc.insn[n].op_str);
                    
                    if (sc.insn[n].address == 0x44fc5cb) {
                        lend = false;   /// test
                    }
                    
                    if (!loadm && IsSubMem(&sc.insn[n],&addr,&mem,&count)) {
                        // New submem
                        if (ltraces) printf("%s *** Add submem 0x%llx(%li)\n",lang->COMM(),addr,count);
                        c->AddSubMem(addr,mem,count);
                        free(mem);
                    }
                    if (IsCall(&sc.insn[n],&addr)) {
                        if (!Excluded(addr)) {
                            // New Function 
                            if (!ncalls) {
                                calls = (struct _call *) malloc(sizeof(struct _call));
                            }
                            else {
                                calls = (struct _call *) realloc(calls,sizeof(struct _call)*(ncalls+1));
                            }
                            calls[ncalls].addr = addr;
                            calls[ncalls].name = NULL;
                            //if (IsNamedFunction(addr,&calls[ncalls].name) || arch->IsSymbolFunction(addr,&calls[ncalls].name)) {
                            if (IsNamedFunction(addr,&calls[ncalls].name)) {
                                if (ltraces) printf("%s *** Add call 0x%llx(%s)\n",lang->COMM(),addr,calls[ncalls].name);
                            }
                            else {
                                if (ltraces) printf("%s *** Add call 0x%llx\n",lang->COMM(),addr);
                            }
                            ncalls++;
                        }
                    }
                    else if (IsJmpIAT(&sc.insn[n],&lib,&func)) {
                        char *tmp = (char *)malloc(1024);
                        sprintf(tmp,"__%s_%s",lib,func);
                        char *p  = tmp;
                        do {
                            p=strchr(p,'.');
                            if (p != NULL) {
                                *p = '_';
                            }
                        }
                        while (p != NULL);
                        while (c->ExistFunctionName(tmp)) {
                            strcat(tmp,"_x");
                        }
                        c->RenameFunction(&sc,tmp,false);
                        free(tmp);
                        free(lib);
                        free(func);
                        lend = true;
                    }
                    else if (IsJcc(&sc.insn[n],&addr)) {
                        // New conditional goto
                        if (ltraces) printf("%s *** Add jcc 0x%llx\n",lang->COMM(),addr);
                        jmps.insert(addr);
                        c->AddLabel(&sc,addr);
                    }
                    //else if(IsJmp(&sc.insn[n],&addr_list,&count)) {
                    else if(IsJmp(sc.insn,n,&addr_list,&count,&anon)) {
                        if (anon) {
                            // Funtion has indirect jmps
                            c->SetAnonJmp(&sc);
                        }
                        for (nn=0;nn<count;nn++) {
                            if (addr_list[nn] != UNDEF_ADDR_JMP) {
                                // New defined goto
                                jmps.insert(addr_list[nn]);
                                c->AddLabel(&sc,addr_list[nn]);
                            }
                        }
                        free (addr_list);
                        lend = true;
                    }
                    else if (IsRet(&sc.insn[n],&bytes)) {
                        c->SetRetBytes(&sc,bytes);
                        lend = true;
                    }
                    else if (IsInt(&sc.insn[n],&addr) || IsEnd(sc.insn,n,sc.count)) {
                        lend = true;
                    }
                    if (lend) {
                        // End Funtion
                        if (ltraces) printf("%s *** End of subcode 0x%llx\n",lang->COMM(),sc.first);
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
                    if (ltraces) printf("%s *** Add subcode 0x%llx %li (parent=%i)\n",lang->COMM(),sc.first,sc.last-sc.first,sc.parent);
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
char *name;

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
            IsNamedFunction(address,&name);
            Code *c = GetCode(NULL,address,name,SUBCODE_TOP);
            c = Include(c);
            //c->Print();
            AnalyzeCode(c);
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
    lstack = false;
}

Butcher::~Butcher() {
    delete lang;
    delete arch;
}
