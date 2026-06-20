#include "base_x64.hpp"
#include "../files/pe.hpp"
#include <capstone/capstone.h>

const char *reg_name(csh handle,int id_reg) {
    if (id_reg == X86_REG_INVALID) {
        return ("");
    }
    return (cs_reg_name(handle,id_reg));
}

int JccInst(cs_insn *insn) {
    switch(insn->id) {
        case X86_INS_JA:
        case X86_INS_JAE:
        case X86_INS_JB:
        case X86_INS_JBE:
        case X86_INS_JE:
        case X86_INS_JECXZ:
        case X86_INS_JG:
        case X86_INS_JGE:
        case X86_INS_JL:
        case X86_INS_JLE:
        case X86_INS_JNE:
        case X86_INS_JNO:
        case X86_INS_JNP:
        case X86_INS_JNS:
        case X86_INS_JO:
        case X86_INS_JP:
        case X86_INS_JRCXZ:
        case X86_INS_JS:
        case X86_INS_LOOP:
        case X86_INS_LOOPE:
        case X86_INS_LOOPNE:
            return (true);
    }
    return (false);
}

int CMoveInst(cs_insn *insn) {
    switch(insn->id) {
        case X86_INS_CMOVA:
        case X86_INS_CMOVAE:
        case X86_INS_CMOVB:
        case X86_INS_CMOVBE:
        case X86_INS_CMOVE:
        case X86_INS_CMOVG:
        case X86_INS_CMOVGE:
        case X86_INS_CMOVL:
        case X86_INS_CMOVLE:
        case X86_INS_CMOVNE:
        case X86_INS_CMOVNO:
        case X86_INS_CMOVNP:
        case X86_INS_CMOVNS:
        case X86_INS_CMOVO:
        case X86_INS_CMOVP:
        case X86_INS_CMOVS:
            return (true);
    }
    return (false);
}

int SetInst(cs_insn *insn) {
    switch (insn->id) {
        case X86_INS_SETA:
        case X86_INS_SETAE:
	    case X86_INS_SETBE:
	    case X86_INS_SETB:
	    case X86_INS_SETE:
	    case X86_INS_SETGE:
	    case X86_INS_SETG:
	    case X86_INS_SETLE:
	    case X86_INS_SETL:
	    case X86_INS_SETNE:
	    case X86_INS_SETNO:
	    case X86_INS_SETNP:
	    case X86_INS_SETNS:
	    case X86_INS_SETO:
	    case X86_INS_SETP:
	    case X86_INS_SETS:
            return (true);
    }
    return (false);
}

int SetFlagInst(cs_insn *insn) {
    switch(insn->id) {
        case X86_INS_SUB:
        case X86_INS_ADD:
        case X86_INS_XOR:
        case X86_INS_ADC:
        case X86_INS_SBB:
        case X86_INS_CMP:
        case X86_INS_TEST:
        case X86_INS_INC:
        case X86_INS_DEC:
        case X86_INS_SHL:
        case X86_INS_SHR:
        case X86_INS_SAL:
        case X86_INS_SAR:
        case X86_INS_ROL:
        case X86_INS_ROR:
        case X86_INS_RCL:
        case X86_INS_RCR:
        case X86_INS_BT:
        case X86_INS_SCASB:
        case X86_INS_SCASW:
        case X86_INS_SCASD:
        case X86_INS_SCASQ:
        case X86_INS_CMPSB:
        case X86_INS_CMPSW:
        case X86_INS_CMPSD:
        case X86_INS_CMPSQ:
        case X86_INS_JMP:
        case X86_INS_CALL:
            return (true);
    }
    return (false);
}

int UseFlagInst(cs_insn *insn) {
    if (JccInst(insn) || SetInst(insn) || CMoveInst(insn)) {
        return (true);
    }
    switch (insn->id) {
        case X86_INS_ADC:
        case X86_INS_SBB:
        case X86_INS_RCL:
        case X86_INS_RCR:
            return (true);
    }
    return (false);
}

int FlagsNotUsed(struct _subcode *sc,int num) {
cs_insn *insn;

    num++;
    while (num < sc->count) {
        insn = &sc->insn[num];
        if (UseFlagInst(insn)) {
            return (false);
        }
        if (SetFlagInst(insn)) {
            return (true);
        }
        num++;
    }
    return (true);
}

int FlagsNotUsedAddress(struct Code *c,uint64_t addr) {
int n,m;
struct _subcode *sc;

    for (n=0;n<c->subcod_count;n++) {
        sc = &c->subcodes[n];
        if ((addr >= sc->first) && (addr <= sc->last)) {
            for (m=0;m<sc->count;m++) {
                if (sc->insn[m].address == addr) {
                    return (FlagsNotUsed(sc,m-1));
                }
            }
            break;
        }
    }
    return (false);
}

int IsRIP(int id) {
    return((id == X86_REG_EIP)||(id == X86_REG_RIP)||(id == X86_REG_IP));
}

int IsSTX(int id) {
    return ((id == X86_REG_ST0) ||
            (id == X86_REG_ST1) ||
            (id == X86_REG_ST2) ||
            (id == X86_REG_ST3) ||
            (id == X86_REG_ST4) ||
            (id == X86_REG_ST5) ||
            (id == X86_REG_ST6));
}

// ---------------------------------------------------------------------------------------------------------

cs_err Base_x64::Cs_open(void) {
    return(cs_open(CS_ARCH_X86, arch->Is32()?CS_MODE_32:CS_MODE_64, &handle));
}

int Base_x64::IsRet(cs_insn *insn,int *bytes) {
    *bytes = 0;
    if (insn->id == X86_INS_RET) {
        if (insn->detail->x86.op_count == 1) {
            *bytes = insn->detail->x86.operands[0].imm;
        }
    }
    return(IsGroup(insn,X86_GRP_RET));
}

int Base_x64::IsCall(cs_insn *insn, uint64_t *addr) {
    if (insn->id == X86_INS_CALL) {
        if (insn->detail->x86.op_count == 1) {
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                *addr = insn->detail->x86.operands[0].imm;
                return (true);
            }
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                    *addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;
                    return (true);
                }
                else if ((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                    (insn->detail->x86.operands[0].mem.index == X86_REG_INVALID)) {
                    /*
                    if (arch->ValidMemory(insn->detail->x86.operands[0].mem.disp)) {
                        // 
                        *addr = insn->detail->x86.operands[0].mem.disp;
                        return (true);
                    }
                    */
                    if (arch->Get_Address_At(insn->detail->x86.operands[0].mem.disp,addr,insn->detail->x86.addr_size*8)) {
                        //if (arch->ValidMemory(*addr)) {
                        if (ValidCode(*addr)) {
                            return (true);
                        }
                    }
                }
            }
        }
    }
    return (false);
}

int Base_x64::IsInt(cs_insn *insn, uint64_t *num) {
    return(IsGroup(insn,X86_GRP_INT));
}

int Base_x64::IsEnd(cs_insn *insn, int n, int count) {
    if (insn[n].id == X86_INS_NOP) {
        if (insn[n].detail->x86.op_count > 0) {
            // nop             word ptr [rax + rax]
            return(true);
        }
    }
    if ((insn[n].size == 2) && !insn[n].bytes[0] && !insn[n].bytes[1]) {
        // 0000           add byte [eax], al
        return (true);
    }
    switch (insn[n].id) {
        case X86_INS_IN:
        case X86_INS_INSB:
        case X86_INS_INSW:
        case X86_INS_INSD:
        case X86_INS_OUT:
        case X86_INS_OUTSB:
        case X86_INS_OUTSW:
        case X86_INS_OUTSD:
            return (true);
    }
    return (false);
}

char *Base_x64::AddREPX(char *buffer,cs_insn *insn) {
char *mem;

    mem = (char *) malloc(strlen(buffer)+256);
    switch (insn->detail->x86.prefix[0]) {
        case X86_PREFIX_REP:
            sprintf(mem,"while rcx != 0 do "
                            "%s;"
                            "rcx = rcx - 1 "
                        "endw",buffer);
            break;
        case X86_PREFIX_REPNE:
            sprintf(mem,"while rcx != 0 do "
                            "%s;"
                            "rcx = rcx - 1;"
                            "if get_zf() then break fi "
                        "endw",buffer);
            break;
        default:
            strcpy(mem,buffer);
            break;
    }
    return (mem);
}

#define VALID_CODE_LENGTH   16

int Base_x64::ValidCode(uint64_t addr) {
uint64_t read;
int lok;
int c;
cs_insn *insn;

    /*
    if (arch->ValidMemory(addr)) {
        return (true);
    }
    */
    lok = false;
    uint8_t *m = arch->GetMemory(addr,VALID_CODE_LENGTH,&read);
    if (m!= NULL) {
        c = cs_disasm(handle, m, VALID_CODE_LENGTH, addr, 0, &insn);
        if (c) {
            cs_free(insn,c);
            lok = true;
        }
        free(m);
    }
    return (lok);
}

#define MAX_JMPS      (1024*10)

int Base_x64::IsJmp(cs_insn *in, int num, uint64_t *addr[],int *count, int *anon) {
int n,b,c;
uint8_t *mem;
uint64_t read,a,d;
cs_insn *insn;

    insn = &in[num];
    c = 0;
    *anon = false;
    switch (insn->id) {
        case X86_INS_JMP:
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                d = insn->detail->x86.operands[0].imm;
                //if (arch->ValidMemory(d)) {
                if (ValidCode(d)) {
                    *addr = (uint64_t *) malloc(sizeof(uint64_t));
                    (*addr)[c++] = d;
                }
            }
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                                (insn->detail->x86.operands[0].mem.index != X86_REG_INVALID) && 
                                (insn->detail->x86.operands[0].mem.scale > 1) && (insn->detail->x86.operands[0].mem.disp)) {
                    // Forward
                    for (n=0;n<MAX_JMPS;n++) {
                        a = insn->detail->x86.operands[0].mem.disp + (insn->detail->x86.operands[0].mem.scale * n);
                        b = insn->detail->x86.addr_size;
                        if (arch->Get_Address_At(a,&d,b*8)) {
                            //if (arch->ValidMemory(d)) {
                            if (ValidCode(d)) {
                                if (c == 0) {
                                    *addr = (uint64_t *) malloc(sizeof(uint64_t));
                                }
                                else {
                                    *addr = (uint64_t *) realloc(*addr,sizeof(uint64_t)*(c+1));
                                }
                                (*addr)[c++] = d;
                            }
                            else {
                                break;
                            }
                        }
                        else {
                            break;
                        }
                    }
                    // Backward
                    if ((insn->address+insn->size) < insn->detail->x86.operands[0].mem.disp) {
                        for (n=-1;n>(-MAX_JMPS);n--) {
                            a = insn->detail->x86.operands[0].mem.disp + (insn->detail->x86.operands[0].mem.scale * n);
                            b = insn->detail->x86.addr_size;
                            if (arch->Get_Address_At(a,&d,b*8)) {
                                //if (arch->ValidMemory(d)) {
                                if (ValidCode(d)) {
                                    if (c == 0) {
                                        *addr = (uint64_t *) malloc(sizeof(uint64_t));
                                    }
                                    else {
                                        *addr = (uint64_t *) realloc(*addr,sizeof(uint64_t)*(c+1));
                                    }
                                    (*addr)[c++] = d;
                                }
                                else {
                                    break;
                                }
                            }
                            else {
                                break;
                            }
                        }
                    }
                }
                else {
                    // Undefined jmp
                    *addr = (uint64_t *) malloc(sizeof(uint64_t));
                    (*addr)[c++] = UNDEF_ADDR_JMP; 
                }
            }
            else {
                // Undefined jmp X86_OP_REG
                *anon = true;
                *addr = (uint64_t *) malloc(sizeof(uint64_t));
                if ((num>0) && (in[num-1].id == X86_INS_POP) && (in[num-1].detail->x86.operands[0].type == X86_OP_REG) &&
                    (in[num-1].detail->x86.operands[0].reg == insn->detail->x86.operands[0].reg)) {
                    // push <addr>
                    // [...]
                    // pop <reg>
                    // jmp <reg>
                    for (n=num-2;n>=0;n--) {
                        if (in[n].id == X86_INS_PUSH) {
                            if (in[n].detail->x86.operands[0].type == X86_OP_IMM) {
                                // push <addr>
                                //if (arch->ValidMemory(in[n].detail->x86.operands[0].imm)) {
                                if (ValidCode(in[n].detail->x86.operands[0].imm)) {
                                    (*addr)[c++] = in[n].detail->x86.operands[0].imm;
                                    break;
                                }
                                else {
                                    // exit
                                    n = -1;
                                    break;
                                }
                            }
                            else {
                                // exit
                                n = -1;
                                break;
                            }
                        }
                    }
                    if (n < 0) {
                        (*addr)[c++] = UNDEF_ADDR_JMP;
                    }
                }
                else {
                    (*addr)[c++] = UNDEF_ADDR_JMP;
                }
            }
            break;
        case X86_INS_LOOP:
        case X86_INS_LOOPE:
        case X86_INS_LOOPNE:
            d = insn->detail->x86.operands[0].imm;
            if (ValidCode(d)) {
                *addr = (uint64_t *) malloc(sizeof(uint64_t));
                (*addr)[c++] = d;
            }
            break;
        case X86_INS_RET:
            // push <addr>
            // [...]
            // ret
            *anon = true;
            *addr = (uint64_t *) malloc(sizeof(uint64_t));
            for (n=num-1;n>=0;n--) {
                if (in[n].id == X86_INS_PUSH) {
                    if (in[n].detail->x86.operands[0].type == X86_OP_IMM) {
                        // push <addr>
                        if (ValidCode(in[n].detail->x86.operands[0].imm)) {
                            (*addr)[c++] = in[n].detail->x86.operands[0].imm;
                            break;
                        }
                        else {
                            // exit
                            n = -1;
                            break;
                        }
                    }
                    else {
                        // exit
                        n = -1;
                        break;
                    }
                }
                else if (   (in[n].id == X86_INS_POP) ||
                            (in[n].id == X86_INS_JMP) ||
                            (in[n].id == X86_INS_RET) ||
                            JccInst(&in[n])) {            
                    // exit
                    n = -1;
                    break;
                }
            }
            if (n < 0) {
                (*addr)[c++] = UNDEF_ADDR_JMP;
            }
            break;
    }
    *count = c;
    return (c);
}

int Base_x64::IsJcc(cs_insn *insn, uint64_t *addr) {
    if (JccInst(insn)) {
        if (insn->detail->x86.op_count == 1) {
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                *addr = insn->detail->x86.operands[0].imm;
                return (true);
            }
        }
    }
    return (false);
}

int Base_x64::IsJmpIAT(cs_insn *insn,char **lib,char **func) {
uint64_t addr;

    if (insn->id == X86_INS_JMP) {
        if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
            if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;
                if (arch->IsImportFunction(addr,lib,func)) {
                    return (true);
                }    
            }
            else if((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                    (insn->detail->x86.operands[0].mem.index == X86_REG_INVALID) && 
                    (insn->detail->x86.operands[0].mem.disp)) {
                addr = insn->detail->x86.operands[0].mem.disp;
                if (arch->IsImportFunction(addr,lib,func)) {
                    return (true);
                }
            }
        }
    }
    return (false);
}

//#define MAX_SUBMEM  (1024*5)

int Base_x64::IsSubMem(cs_insn *insn, uint64_t *addr, uint8_t **mem, int *count) {
uint64_t read,maddr;
int n,b;

    for (n=0;n<insn->detail->x86.op_count;n++) {
        if (insn->detail->x86.operands[n].type == X86_OP_IMM) {
            if ((insn->id == X86_INS_MOV) ||
                (insn->id == X86_INS_MOVZX)) {
                maddr = insn->detail->x86.operands[n].imm;
                *mem = arch->GetMemUtil(maddr,addr,count);
                if (*mem != NULL) {
                    return (true);
                }
            }
        }
        else if (insn->detail->x86.operands[n].type == X86_OP_MEM) {
            if (IsRIP(insn->detail->x86.operands[n].mem.base)) {
                // mov rax, qword ptr [**rip** + 0x1dc97]
                *addr = insn->address + insn->size + insn->detail->x86.operands[n].mem.disp;
                b = insn->detail->x86.addr_size;
                *mem = arch->GetMemory(*addr,b,&read);
                if (*mem != NULL) {
                    if (read == b) {
                        *count = read;
                        return (true);
                    }
                    free(*mem);
                }
            }
            else if((insn->detail->x86.operands[n].mem.base == X86_REG_INVALID) && (insn->detail->x86.operands[n].mem.index == X86_REG_INVALID)) {
                // mov             al, byte ptr [0x44fc634]
                *addr = insn->detail->x86.operands[n].mem.disp;
                //n = insn->detail->x86.addr_size;
                b = insn->detail->x86.operands[0].size;
                *mem = arch->GetMemory(*addr,b,&read);
                if (*mem != NULL) {
                    if (read == b) {
                        *count = read;
                        return (true);
                    }
                    free(*mem);
                }
            }
            else if((insn->detail->x86.operands[n].mem.base == X86_REG_INVALID) && (insn->detail->x86.operands[n].mem.index != X86_REG_INVALID)) {
                //if ((insn->id != X86_INS_JMP) && (insn->id != X86_INS_LEA)) {
                // xor		esi, dword ptr [edi*4 + 0x1fd2534]
                b = insn->detail->x86.operands[0].size;
                *addr = insn->detail->x86.operands[n].mem.disp;
                *mem = arch->GetMemory(*addr,1024*b,&read);
                if (*mem != NULL) {
                    *count = read;
                    return (true);
                }
                free(*mem);
                //}
            }
        }
    }
    return (false);
}

// ---------------------------------------------------------------------------------------------------------

void Base_x64::PrintLine(cs_insn *insn,int indent,const char *format,...) {
char buffer[MAX_STR_OP];
char out[MAX_STR_OP];
char *tmp;

    va_list argptr;
    va_start(argptr, format);
    //vsprintf(buffer,format, argptr);
    vsnprintf(buffer,MAX_STR_OP,format,argptr);
    va_end(argptr);
    
    strcpy(out,buffer);
    /*
    for (int n=0;n<indent;n++) {
        strcat (out,lang_x64->INDENT());
    }
    strcat (out,buffer);
    */
    for (int n=0;n<indent;n++) {
        tmp = lang_x64->Indent(out);
        strcpy(out,tmp);
    }
    
    char *p = out;
    for (int n=0;n<strlen(out);n++) {
        if (out[n] == '\n') {
            p = out+n+1;
        }
    }
    
    //while (strlen(buffer) < lang_x64->COMM_SEP) strcat(buffer," ");
    if ((insn == NULL) || !lasm) {
        lang_x64->PrintF("%s\n", out);
    }
    else {
        while (strlen(p) < lang_x64->COMM_SEP()) strcat(p," ");
        lang_x64->PrintF("%s%s 0x%llx:\t%s\t\t%s\n", out, lang_x64->COMM(), insn->address, insn->mnemonic,insn->op_str);
    }
}

char *Base_x64::GetLabel(uint64_t addr) {
char *buffer;
char *name;

    buffer = (char *) malloc(256);
    if (IsNamedFunction(addr,&name)) {
        sprintf(buffer,lang_x64->E_LABEL_NAME(),name);
        free(name);
    }
    else {
        sprintf(buffer,lang_x64->E_LABEL_ADDR(),addr);
    }
    return(buffer);
}

char *Base_x64::GetGoto(uint64_t addr) {
char *buffer;
char *name;

    buffer = (char *) malloc(256);
    name = GetLabel(addr);
    sprintf(buffer,lang_x64->E_GOTO(),name);
    free(name);
    return(buffer);
}

char *Base_x64::GetGotoJCC(uint64_t addr,char *cond) {
char *buffer;
char *name;

    buffer = (char *) malloc(256);
    name = GetLabel(addr);
    sprintf(buffer,lang_x64->E_JCC_GOTO(),cond,name);
    free(name);
    return(buffer);
}


//void Base_x64::PrintLabel(Code *c,uint64_t addr) {
void Base_x64::PrintLabel(Code *c,struct _subcode *sc,uint64_t addr) {
char *name;
//struct _subcode *p; 

    //if (std::find(c->labels.begin(), c->labels.end(), addr) != c->labels.end()) {
    //p = c->GetParent(sc);
    //if (std::find(c->labels.begin(), c->labels.end(), addr) != c->labels.end()) {
    //if (std::find(p->labels.begin(), p->labels.end(), addr) != p->labels.end()) {
    if (c->ExistLabel(sc,addr)) {
        // TODO: NamedFunction
        /*
        if (IsNamedFunction(addr,&name)) {
            printf(lang_x64->E_LABEL_NAME,name);
            free(name);
        }
        else {
        */
        //    printf(lang_x64->E_LABEL(),addr);
        //}
        name = GetLabel(addr);
        lang_x64->PrintF(lang_x64->E_LABEL(),name);
        free (name);
    }
}

int Base_x64::IsFSGS(cs_insn *insn) {
int n;

    for (n=0;n<insn->detail->x86.op_count;n++) {
        if (insn->detail->x86.operands[n].type == X86_OP_MEM) {
            if ((insn->detail->x86.operands[n].mem.segment == X86_REG_FS) || (insn->detail->x86.operands[n].mem.segment == X86_REG_GS)) {
                return (true);
            }
        }
    }
    return (false);
}

int Base_x64::PrintExtra(Code *c,struct _subcode *sc,int num) {
cs_insn *insn;
uint64_t read,d,raddr;
char *reg0,*reg1,*mstr;
uint64_t addr;
char *lib,*func,*name;
uint8_t *mem;
int n,b,ldone,lj;
int bits;
char buffer[1024];

    insn = &sc->insn[num];
    bits = insn->detail->x86.addr_size*8;
    if (insn->address == 0x771b36) {
        // test
        n = 0;
    }
    if ((sc->parent == SUBCODE_TOP) && !num) {
        // First instruction, push rip/eip
        reg0 = lang_x64->Translate(handle,"push(bits,raddr)",insn,true);
        if (reg0 != NULL) {
            PrintLine(insn,1,reg0);
            free(reg0);
        }
    }
    switch (insn->id) {
        case X86_INS_NOP:
            PrintLine(insn,1,"");
            num++;
            break;
        case X86_INS_WAIT:
            PrintLine(insn,1,"");
            num++;
            break;
        case X86_INS_PAUSE:
            PrintLine(insn,1,"");
            num++;
            break;
        case X86_INS_LEAVE:
            reg0 = lang_x64->Translate(handle,  "rsp = rbp;"
                                                "rbp = pop(bits)",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            break;
        case X86_INS_RET:
            // ret
            //reg0 = lang_x64->Translate(handle,"pop(bits)",insn,true);
            reg0 = lang_x64->Translate(handle,"anonjmp = pop(bits)",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                free(reg0);
            }
            if (insn->detail->x86.op_count > 0) {
                reg0 = lang_x64->Translate(handle,"rsp = rsp + op0",insn,true);
                if (reg0 != NULL) {
                    PrintLine(insn,1,reg0);
                    free(reg0);
                }
            }
            //PrintLine(insn,1,lang_x64->E_RETURN());
            reg0 = lang_x64->Translate(handle,"goto anonlabel",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                free(reg0);
            }
            num++;
            break;
        case X86_INS_JMP:
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                reg0 = GetGoto(insn->detail->x86.operands[0].imm);
                //PrintLine(insn,1,lang_x64->E_GOTO(),insn->detail->x86.operands[0].imm);
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if (IsJmpIAT(insn,&lib,&func)) {
                    PrintLine(insn,1,lang_x64->E_JMP_FROM_IAT(),lib,func);
                    free(lib);
                    free(func);
                    num++;
                }
                else if((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.index != X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.scale > 1) && (insn->detail->x86.operands[0].mem.disp)) {
                    // jmp             dword ptr [eax*4 + 0x44fcb89]
                    reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].mem.index);
                    lj = false;
                    // Forward
                    for (n=0;n<MAX_JMPS;n++) {
                        addr = insn->detail->x86.operands[0].mem.disp + (insn->detail->x86.operands[0].mem.scale * n);
                        b = insn->detail->x86.addr_size;
                        if (arch->Get_Address_At(addr,&d,b*8)) {
                            //if (arch->ValidMemory(d)) {
                            if (ValidCode(d)) {
                                PrintLine(insn,1,(!lj)?lang_x64->E_IF_R_EQ_I():lang_x64->E_ELIF_R_EQ_I(),reg0,n);
                                lj = true;
                                //PrintLine(insn,2,lang_x64->E_GOTO(),d);
                                reg1 = GetGoto(d);
                                PrintLine(insn,2,reg1);
                                free(reg1);
                                PrintLine(insn,1,lang_x64->E_ENDIF());
                            }
                            else {
                                break;
                            }
                        }
                        else {
                            break;
                        }
                    }
                    // Backward
                    if ((insn->address+insn->size) < insn->detail->x86.operands[0].mem.disp) {
                        for (n=-1;n>(-MAX_JMPS);n--) {
                            addr = insn->detail->x86.operands[0].mem.disp + (insn->detail->x86.operands[0].mem.scale * n);
                            b = insn->detail->x86.addr_size;
                            if (arch->Get_Address_At(addr,&d,b*8)) {
                                //if (arch->ValidMemory(d)) {
                                if (ValidCode(d)) {
                                    PrintLine(insn,1,(!lj)?lang_x64->E_IF_R_EQ_I():lang_x64->E_ELIF_R_EQ_I(),reg0,n);
                                    lj = true;
                                    //PrintLine(insn,2,lang_x64->E_GOTO(),d);
                                    reg1 = GetGoto(d);
                                    PrintLine(insn,2,reg1);
                                    free(reg1);
                                    PrintLine(insn,1,lang_x64->E_ENDIF());
                                }
                                else {
                                    break;
                                }
                            }
                            else {
                                break;
                            }
                        }
                    }
                    if (lj) {
                        // else panic()
                        PrintLine(insn,1,lang_x64->E_ELSE());
                        PrintLine(insn,2,lang_x64->E_PANIC_JMP_INDEX(),reg0);
                        PrintLine(insn,1,lang_x64->E_ENDIF());
                    }
                    free(reg0);
                    //PrintLine(insn,1,lang_x64->E_ENDIF);
                    num++;
                }
            }
            else if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang_x64->Translate(handle,  "anonjmp = op0;"
                                                    "goto anonlabel",insn,true);
                if (reg0 != NULL) {
                    PrintLine(insn,1,reg0);
                    free(reg0);
                    num++;
                }   
            }
            break;
        case X86_INS_JE:
            // (ZF=1)
            reg0 = lang_x64->Translate(handle,TEST_E,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JNE:
            // (ZF=0)
            reg0 = lang_x64->Translate(handle,TEST_NE,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JA:
            // (CF=0 and ZF=0)
            reg0 = lang_x64->Translate(handle,TEST_A,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JAE:
            // (CF=0)
            reg0 = lang_x64->Translate(handle,TEST_AE,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JL:
            // (SF!=OF)
            reg0 = lang_x64->Translate(handle,TEST_L,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JLE:
            // (ZF=1 or SF!=OF)
            reg0 = lang_x64->Translate(handle,TEST_LE,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JG:
            // (ZF=0 and SF=OF)
            reg0 = lang_x64->Translate(handle,TEST_G,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JGE:
            // (SF=OF)
            reg0 = lang_x64->Translate(handle,TEST_GE,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JO:
            // (OF=1)
            reg0 = lang_x64->Translate(handle,TEST_O,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JNO:
            // (OF=0)
            reg0 = lang_x64->Translate(handle,TEST_NO,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JS:
            // (SF=1)
            reg0 = lang_x64->Translate(handle,TEST_S,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JNS:
            // (SF=0)
            reg0 = lang_x64->Translate(handle,TEST_NS,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JB:
            // (CF=1)
            reg0 = lang_x64->Translate(handle,TEST_B,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_JBE:
            // (CF=1 or ZF=1)
            reg0 = lang_x64->Translate(handle,TEST_BE,insn,false);
            reg1 = GetGotoJCC(insn->detail->x86.operands[0].imm,reg0);
            PrintLine(insn,1,reg1);
            free(reg1);
            free(reg0);
            num++;
            break;
        case X86_INS_LOOP:
            reg0 = lang_x64->Translate(handle,  "rcx = rcx - 1;"
                                                "if rcx > 0 then goto op0 fi",insn,false);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_PUSH:
            /*
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
                PrintLine(insn,1,lang_x64->E_PUSH,lang_x64->ptr(insn->detail->x86.operands[0]),reg0);
                free(reg0);
                num++;
            }
            */
            if (IsFSGS(insn)) {
                reg0 = lang_x64->Translate(handle,"push(bits,0)",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,"push(bits,op0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_POP:
            /*
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
                PrintLine(insn,1,lang_x64->E_POP,reg0,lang_x64->ptr(insn->detail->x86.operands[0]));
                free(reg0);
                num++;
            }
            */
            if (IsFSGS(insn)) {
                reg0 = lang_x64->Translate(handle,"pop(bits)",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,"op0 = pop(bits)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_ADD:
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = op0 + op1",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "add_cf(bits,op0,op1);"
                                                    "add_of(bits,sop0,sop1);"
                                                    "op0 = op0 + op1;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_XADD:
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,  "push(bits,op0);"
                                                    "op0 = op0 + op1;"
                                                    "op1 = pop(bits)",insn,true);
                if (reg0 != NULL) {
                    PrintLine(insn,1,reg0);
                    num++;
                    free(reg0);
                }
            }
            break;
        case X86_INS_ADC:
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                //reg0 = lang_x64->Translate(handle,".op0 = op0 + op1 + num_cf()",insn,true);
                reg0 = lang_x64->Translate(handle,  "if get_cf() then "
                                                        "op0 = op0 + op1 + 1 "
                                                    "else "
                                                        "op0 = op0 + op1 "
                                                    "fi",insn,true);
            }
            else {
                //reg0 = NULL;
                reg0 = lang_x64->Translate(handle,  "if get_cf() then "
                                                        "add_cf(bits,op0,op1+1);"
                                                        "add_of(bits,sop0,sop1+1);"
                                                        "op0 = op0 + (op1 + 1) "
                                                    "else "
                                                        "add_cf(bits,op0,op1);"
                                                        "add_of(bits,sop0,sop1);"
                                                        "op0 = op0 + op1 "
                                                    "fi;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_INC:
            /*
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                if (FlagsNotUsed(sc,num)) {
                    reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
                    PrintLine(insn,1,lang_x64->E_ADD_RR,reg0,reg0,"1");
                    num++;
                    free(reg0);
                }
            }
            */
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = op0 + 1",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "add_cf(bits,op0,1);"
                                                    "add_of(bits,sop0,1);"
                                                    "op0 = op0 + 1;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SUB:
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = op0 - op1",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "cf(op1 > op0);"
                                                    "sub_of(bits,sop0,sop1);"
                                                    "op0 = op0 - op1;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SBB:
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                //reg0 = lang_x64->Translate(handle,".op0 = op0 - (op1 + num_cf())",insn,true);
                reg0 = lang_x64->Translate(handle,  "if get_cf() then "
                                                        "op0 = op0 - (op1 + 1) "
                                                    "else "
                                                        "op0 = op0 - op1 "
                                                    "fi",insn,true);
            }
            else {
                //reg0 = NULL;
                reg0 = lang_x64->Translate(handle,  "if get_cf() then "
                                                        "cf((op1+1) > op0);"
                                                        "sub_of(bits,sop0,sop1+1);"
                                                        "op0 = op0 - (op1 + 1) "
                                                    "else "
                                                        "cf(op1 > op0);"
                                                        "sub_of(bits,sop0,sop1);"
                                                        "op0 = op0 - op1 "
                                                    "fi;"
                                                    ".zf(op0 == 0);"
                                                    ".sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_DEC:
            /*
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                if (FlagsNotUsed(sc,num)) {
                    reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
                    PrintLine(insn,1,lang_x64->E_SUB_RR,reg0,reg0,"1");
                    num++;
                    free(reg0);
                }
            }
            */
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = op0 - 1",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "cf(op0 == 0);"
                                                    "sub_of(bits,sop0,1);"
                                                    "op0 = op0 - 1;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMP:
            ldone = false;
            if ((num+1) < sc->count) {
                cs_insn *next = &sc->insn[num+1];
                switch (next->id) {
                    case X86_INS_JA:
                        // (CF=0 and ZF=0)
                        if (FlagsNotUsed(sc,num+1) && FlagsNotUsedAddress(c,next->detail->x86.operands[0].imm)) {
                            // "if (%s > %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 > op1",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            PrintLabel(c,sc,next->address);
                            //PrintLine(next,1,lang_x64->E_JCC_GOTO(),reg0,next->detail->x86.operands[0].imm);
                            reg1 = GetGotoJCC(next->detail->x86.operands[0].imm,reg0);
                            PrintLine(next,1,reg1);
                            free(reg1);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JGE:
                        // (SF=OF)
                        if (FlagsNotUsed(sc,num+1) && FlagsNotUsedAddress(c,next->detail->x86.operands[0].imm)) {
                            // "if (%s >= %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"sop0 >= sop1",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            PrintLabel(c,sc,next->address);
                            //PrintLine(next,1,lang_x64->E_JCC_GOTO(),reg0,next->detail->x86.operands[0].imm);
                            reg1 = GetGotoJCC(next->detail->x86.operands[0].imm,reg0);
                            PrintLine(next,1,reg1);
                            free(reg1);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JE:
                        // (ZF=1)
                        if (FlagsNotUsed(sc,num+1) && FlagsNotUsedAddress(c,next->detail->x86.operands[0].imm)) {
                            // "if (%s == %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 == op1",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            PrintLabel(c,sc,next->address);
                            //PrintLine(next,1,lang_x64->E_JCC_GOTO(),reg0,next->detail->x86.operands[0].imm);
                            reg1 = GetGotoJCC(next->detail->x86.operands[0].imm,reg0);
                            PrintLine(next,1,reg1);
                            free(reg1);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JNE:
                        // (ZF=0)
                        if (FlagsNotUsed(sc,num+1) && FlagsNotUsedAddress(c,next->detail->x86.operands[0].imm)) {
                            // "if (%s != %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 != op1",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            PrintLabel(c,sc,next->address);
                            //PrintLine(next,1,lang_x64->E_JCC_GOTO(),reg0,next->detail->x86.operands[0].imm);
                            reg1 = GetGotoJCC(next->detail->x86.operands[0].imm,reg0);
                            PrintLine(next,1,reg1);
                            free(reg1);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JBE:
                        // (CF=1 or ZF=1)
                        if (FlagsNotUsed(sc,num+1) && FlagsNotUsedAddress(c,next->detail->x86.operands[0].imm)) {
                            // "if (%s <= %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 <= op1",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            PrintLabel(c,sc,next->address);
                            //PrintLine(next,1,lang_x64->E_JCC_GOTO(),reg0,next->detail->x86.operands[0].imm);
                            reg1 = GetGotoJCC(next->detail->x86.operands[0].imm,reg0);
                            PrintLine(next,1,reg1);
                            free(reg1);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JB:
                        // (CF=1)
                        if (FlagsNotUsed(sc,num+1) && FlagsNotUsedAddress(c,next->detail->x86.operands[0].imm)) {
                            // "if (%s < %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 < op1",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            PrintLabel(c,sc,next->address);
                            //PrintLine(next,1,lang_x64->E_JCC_GOTO(),reg0,next->detail->x86.operands[0].imm);
                            reg1 = GetGotoJCC(next->detail->x86.operands[0].imm,reg0);
                            PrintLine(next,1,reg1);
                            free(reg1);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                }
            }
            if (!ldone) {
                // The OF, SF, ZF, AF, CF, and PF flags
                if (FlagsNotUsed(sc,num)) {
                    reg0 = strdup("");
                }
                else {
                    reg0 = lang_x64->Translate(handle,  "cf(op1 > op0);"
                                                        "sub_of(bits,sop0,sop1);"
                                                        "zf((op0-op1) == 0);"
                                                        "sf((sop0-sop1) < 0)",insn,true);
                }
                if (reg0 != NULL) {
                    PrintLine(insn,1,reg0);
                    num++;
                    free(reg0);
                }
            }
            break;
        case X86_INS_CMPXCHG:
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,  "if rax == op0 then "
                                                        "op0 = op1 "
                                                    "else "
                                                        "rax = op0 "
                                                    "fi",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "zf(rax == op0);"
                                                    "cf(op0 > rax);"
                                                    "sf((s_rax-sop0) < 0);"
                                                    "sub_of(bits,s_rax,sop0);"
                                                    "if rax == op0 then "
                                                        "op0 = op1 "
                                                    "else "
                                                        "rax = op0 "
                                                    "fi",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_OR:
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = op0 | op1",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "cf(false);"
                                                    "of(false);"
                                                    "op0 = op0 | op1;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;       
        case X86_INS_XOR:
            if ((insn->detail->x86.operands[0].type == X86_OP_REG) && (insn->detail->x86.operands[1].type == X86_OP_REG) &&
                (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg) && FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = 0",insn,true);
            }
            else if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = op0 ^ op1",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "cf(false);"
                                                    "of(false);"
                                                    "op0 = op0 ^ op1;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_NOT:
            reg0 = lang_x64->Translate(handle,"op0 = not(bits,op0)",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_NEG:
            //The CF flag set to 0 if the source operand is 0; otherwise it is set to 1. The OF, SF, ZF, AF, and PF flags are set according to the result.
            reg0 = lang_x64->Translate(handle,  "cf(op0 != 0);"
                                                "op0 = neg(bits,op0);"
                                                "zf(op0 == 0);"
                                                "sf(sop0 < 0)",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_TEST:
            ldone = false;
            if ((num+1) < sc->count) {
                cs_insn *next = &sc->insn[num+1];
                switch (next->id) {
                    case X86_INS_JNE:
                        // (ZF=0)
                        if (FlagsNotUsed(sc,num+1) && FlagsNotUsedAddress(c,next->detail->x86.operands[0].imm)) {
                            // "if (%s != 0) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"(op0&op1) != 0",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            PrintLabel(c,sc,next->address);
                            //PrintLine(next,1,lang_x64->E_JCC_GOTO(),reg0,next->detail->x86.operands[0].imm);
                            reg1 = GetGotoJCC(next->detail->x86.operands[0].imm,reg0);
                            PrintLine(next,1,reg1);
                            free(reg1);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JE:
                        // (ZF=1)
                        if (FlagsNotUsed(sc,num+1) && FlagsNotUsedAddress(c,next->detail->x86.operands[0].imm)) {
                            // "if (%s == 0) goto label_0x%llx;"
                            reg0 = lang_x64->Translate(handle,"(op0&op1) == 0",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            PrintLabel(c,sc,next->address);
                            //PrintLine(next,1,lang_x64->E_JCC_GOTO(),reg0,next->detail->x86.operands[0].imm);
                            reg1 = GetGotoJCC(next->detail->x86.operands[0].imm,reg0);
                            PrintLine(next,1,reg1);
                            free(reg1);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JLE:
                        // (ZF=1 or SF!=OF)
                        if (FlagsNotUsed(sc,num+1) && FlagsNotUsedAddress(c,next->detail->x86.operands[0].imm)) {
                            // "if (%s <= 0) goto label_0x%llx;"
                            reg0 = lang_x64->Translate(handle,"(sop0&sop1) <= 0",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            PrintLabel(c,sc,next->address);
                            //PrintLine(next,1,lang_x64->E_JCC_GOTO(),reg0,next->detail->x86.operands[0].imm);
                            reg1 = GetGotoJCC(next->detail->x86.operands[0].imm,reg0);
                            PrintLine(next,1,reg1);
                            free(reg1);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                }
            }
            if (!ldone) {
                // The OF and CF flags are set to 0. The SF, ZF, and PF flags are set
                if (FlagsNotUsed(sc,num)) {
                    reg0 = strdup("");
                }
                else {
                    reg0 = lang_x64->Translate(handle,  "cf(false);"
                                                        "of(false);"
                                                        "zf((op0&op1) == 0);"
                                                        "sf((sop0&sop1) < 0)",insn,true);
                }
                if (reg0 != NULL) {
                    PrintLine(insn,1,reg0);
                    num++;
                    free(reg0);
                }
            }
            break;
        case X86_INS_AND:
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = op0 & op1",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "cf(false);"
                                                    "of(false);"
                                                    "op0 = op0 & op1;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_BT:
            reg0 = lang_x64->Translate(handle,"cf(op0 & (1 << op1))",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SHR:
            // TODO: OF when op1 == 1
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = op0 >> op1",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "tmp2 = op1;"
                                                    "while tmp2 > 0 do "
                                                        "tmp = (op0 & 1);"
                                                        "op0 = op0 >> 1;"
                                                        "cf(tmp > 0);"
                                                        "tmp2 = tmp2 - 1"
                                                    "endw;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SAR:
            reg0 = lang_x64->Translate(handle,  "tmp2 = op1;"
                                                "tmp = op0 >> (bits0-1);"
                                                "tmp = tmp << (bits0-1);"
                                                "while tmp2 > 0 do "
                                                    "cf((op0 & 1) > 0);"
                                                    "op0 = op0 >> 1;"
                                                    "op0 = op0 | tmp;"
                                                    "tmp2 = tmp2 - 1"
                                                "endw;"
                                                "zf(op0 == 0);"
                                                "sf(sop0 < 0)",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;    
        case X86_INS_SHL:
        case X86_INS_SAL:
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,"op0 = op0 << op1",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "tmp2 = op1;"
                                                    "while tmp2 > 0 do "
                                                        "tmp = op0;"
                                                        "tmp = tmp << 1;"
                                                        "op0 = tmp & mask(bits0);"
                                                        "tmp = tmp >> bits0;"
                                                        "cf(tmp > 0);"
                                                        "tmp2 = tmp2 - 1"
                                                    "endw;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SHLD:
            // TODO: OF when op2 == 1
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,  "tmp = op0;"
                                                    "tmp = tmp << bits0;"
                                                    "tmp = tmp | op1;"
                                                    "tmp = tmp << op2;"
                                                    "op0 = tmp >> bits0",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "tmp2 = op2;"
                                                    "while tmp2 > 0 do "
                                                        "tmp = op0 >> (bits0-1);"
                                                        "cf(tmp > 0);"
                                                        "tmp = op0;"
                                                        "tmp = tmp << bits0;"
                                                        "tmp = tmp | op1;"
                                                        "tmp = tmp << 1;"
                                                        "op0 = tmp >> bits0;"
                                                        "tmp2 = tmp2 - 1"
                                                    "endw;"
                                                    "zf(op0 == 0);"
                                                    "sf(sop0 < 0)",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_ROL:
            // TODO: OF when op1 == 1
            reg0 = lang_x64->Translate(handle,  "tmp2 = op1;"
                                                "while tmp2 > 0 do "
                                                    "tmp = op0;"
                                                    "tmp = tmp << 1;"
                                                    "op0 = tmp & mask(bits0);"
                                                    "tmp = tmp >> bits0;"
                                                    "cf(tmp > 0);"
                                                    "op0 = op0 | tmp;"
                                                    "tmp2 = tmp2 - 1"
                                                "endw",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_ROR:
            // TODO: OF when op1 == 1
            reg0 = lang_x64->Translate(handle,  "tmp2 = op1;"
                                                "while tmp2 > 0 do "
                                                    "cf(op0 & 1);"
                                                    "op0 = op0 >> 1;"
                                                    "if get_cf() then op0 = op0 | (1 << (bits0-1)) fi;"
                                                    "tmp2 = tmp2 - 1"
                                                "endw",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_RCL:
            // TODO: OF when op1 == 1
            reg0 = lang_x64->Translate(handle,  "tmp2 = op1;"
                                                "while tmp2 > 0 do "
                                                    "tmp = op0;"
                                                    "tmp = tmp << 1;"
                                                    "if get_cf() then tmp = tmp | 1 fi;"
                                                    "op0 = tmp & mask(bits0);"
                                                    "tmp = tmp >> bits0;"
                                                    "cf(tmp > 0);"
                                                    "tmp2 = tmp2 - 1"
                                                "endw",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_RCR:
            // TODO: OF when op1 == 1
            reg0 = lang_x64->Translate(handle,  "tmp2 = op1;"
                                                "while tmp2 > 0 do "
                                                    "if get_cf() then tmp = 1 else tmp = 0 fi;"
                                                    "tmp = tmp << bits0;" 
                                                    "tmp = tmp | op0;"
                                                    "cf(tmp & 1);"
                                                    "tmp = tmp >> 1;"
                                                    "op0 = tmp & mask(bits0);"
                                                    "tmp2 = tmp2 - 1"
                                                "endw",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_MUL:
            // TODO: ax = al * mem
            reg0 = lang_x64->Translate(handle,  "tmp = rax;"
                                                "tmp = tmp * op0;"
                                                "rax = tmp & mask(bits0);"
                                                "rdx = (tmp >> bits0) & mask(bits0)",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_IMUL:
            // TODO: ax = al * mem
            switch (insn->detail->x86.op_count) {
                case 1:
                    //reg0 = lang_x64->Translate(handle,"s_rax = s_rax * sop0; rdx = 0",insn,true);
                    reg0 = lang_x64->Translate(handle,  "tmp = s_rax;"
                                                        "tmp = tmp * sop0;"
                                                        "rax = tmp & mask(bits0);"
                                                        "rdx = (tmp >> bits0) & mask(bits0)",insn,true);
                    if (reg0 != NULL) {
                        PrintLine(insn,1,reg0);
                        num++;
                        free(reg0);
                    }
                    break;
                case 2:
                    reg0 = lang_x64->Translate(handle,"sop0 = sop0 * sop1",insn,true);
                    if (reg0 != NULL) {
                        PrintLine(insn,1,reg0);
                        num++;
                        free(reg0);
                    }
                    break;
                case 3:
                    reg0 = lang_x64->Translate(handle,"sop0 = sop1 * sop2",insn,true);
                    if (reg0 != NULL) {
                        PrintLine(insn,1,reg0);
                        num++;
                        free(reg0);
                    }
                    break;
            }
            break;
        case X86_INS_DIV:
            /*
            reg0 = lang_x64->Translate(handle,  "rdx = rax % op0;"
                                                "rax = rax / op0",insn,true);
            */
            reg0 = lang_x64->Translate(handle,  "tmp = rdx;"
                                                "tmp = tmp << bits0;"
                                                "tmp = tmp | rax;"
                                                "rdx = tmp % op0;"
                                                "rax = udiv(tmp,op0)",insn,true);
                                                //"rax = tmp / op0",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_IDIV:
            /*
            reg0 = lang_x64->Translate(handle,  "s_rdx = s_rax % sop0;"
                                                // TODO: delete idiv
                                                "s_rax = sdiv(s_rax,sop0)"
                                                //"s_rax = s_rax / sop0"
                                                ,insn,true);
            */
            reg0 = lang_x64->Translate(handle,  "tmp = rdx;"
                                                "tmp = tmp << bits0;"
                                                "tmp = tmp | rax;"
                                                "s_rdx = tmp % sop0;"
                                                "s_rax = sdiv(tmp,sop0)",insn,true);
                                                //"s_rax = tmp / sop0",insn,true);
            //reg0 = lang_x64->Translate(handle,".s_rdx = s_rax % sop0;:.s_rax = s_rax / sop0",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CBW:
        case X86_INS_CWDE:
        case X86_INS_CDQE:
            reg0 = lang_x64->Translate(handle,"s_rax = s_eax",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CWD:
        case X86_INS_CDQ:
        case X86_INS_CQO:
            /*
            reg0 = lang_x64->Translate(handle,  "tmp = s_rax;"
                                                "rax = tmp&mask(bits);"
                                                "tmp = tmp >> bits;"
                                                "rdx = tmp"
                                                ,insn,true);
            */
            reg0 = lang_x64->Translate(handle,  "if s_rax < 0 then "
                                                    "rdx = mask(bits) "
                                                "else "
                                                    "rdx = 0 "
                                                "fi"
                                                ,insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_LEA:
            reg0 = lang_x64->Translate(handle,"op0 = mem1",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETNE:
            reg0 = lang_x64->Translate(handle,"if " TEST_NE " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETE:
            reg0 = lang_x64->Translate(handle,"if " TEST_E " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETB:
            reg0 = lang_x64->Translate(handle,"if " TEST_B " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETBE:
            reg0 = lang_x64->Translate(handle,"if " TEST_BE " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETG:
            reg0 = lang_x64->Translate(handle,"if " TEST_G " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETGE:
            reg0 = lang_x64->Translate(handle,"if " TEST_GE " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETL:
            reg0 = lang_x64->Translate(handle,"if " TEST_L " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETLE:
            reg0 = lang_x64->Translate(handle,"if " TEST_LE " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETS:
            reg0 = lang_x64->Translate(handle,"if " TEST_S " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETNS:
            reg0 = lang_x64->Translate(handle,"if " TEST_NS " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETA:
            reg0 = lang_x64->Translate(handle,"if " TEST_A " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETAE:
            reg0 = lang_x64->Translate(handle,"if " TEST_AE " then op0 = 1 else op0 = 0 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVA:
            reg0 = lang_x64->Translate(handle,"if " TEST_A " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVAE:
            reg0 = lang_x64->Translate(handle,"if " TEST_AE " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVG:
            reg0 = lang_x64->Translate(handle,"if " TEST_G " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVGE:
            reg0 = lang_x64->Translate(handle,"if " TEST_GE " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVL:
            reg0 = lang_x64->Translate(handle,"if " TEST_L " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVLE:
            reg0 = lang_x64->Translate(handle,"if " TEST_LE " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVB:
            reg0 = lang_x64->Translate(handle,"if " TEST_B " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVBE:
            reg0 = lang_x64->Translate(handle,"if " TEST_BE " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVE:
            reg0 = lang_x64->Translate(handle,"if " TEST_E " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVNE:
            reg0 = lang_x64->Translate(handle,"if " TEST_NE " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVS:
            reg0 = lang_x64->Translate(handle,"if " TEST_S " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CMOVNS:
            reg0 = lang_x64->Translate(handle,"if " TEST_NS " then op0 = op1 fi",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_XCHG:
            reg0 = lang_x64->Translate(handle,  "tmp = op0;"
                                                "op0 = op1;"
                                                "op1 = tmp",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_PSHUFD:
            reg0 = lang_x64->Translate(handle,"op0 = pshufd(op1,op2)",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_MOVABS:
        case X86_INS_MOVAPS:
        case X86_INS_MOVUPS:
        case X86_INS_MOVD:
        case X86_INS_MOVQ:
        case X86_INS_MOVZX:
        case X86_INS_MOV:
            if (IsFSGS(insn)) {
                reg0 = lang_x64->Translate(handle,"",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,"op0 = op1",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,1,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_MOVSX:
        case X86_INS_MOVSXD:
            reg0 = lang_x64->Translate(handle,"sop0 = sop1",insn,true);
            PrintLine(insn,1,reg0);
            num++;
            free(reg0);
            break;
        case X86_INS_CALL:
            /*
            raddr = insn->address+insn->size;
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                // call            0x180002240
                addr = insn->detail->x86.operands[0].imm;
                if (!Excluded(addr)) {
                    if (ValidCode(addr)) {
                        name = c->GetFunctionName(addr);
                        if (name != NULL) {
                            PrintLine(insn,1,lang_x64->E_FUNC_NAME(),name,raddr);
                            free(name);   
                        }
                        else {
                            PrintLine(insn,1,lang_x64->E_FUNC_ADDR(),addr,raddr);
                        }
                        num++;
                    }
                }
                else {
                    PrintLine(insn,0,lang_x64->E_SPACE());
                    num++;
                }
            } 
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                // Memory
                if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                    addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;
                    if (arch->IsImportFunction(addr,&lib,&func)) {
                        PrintLine(insn,1,lang_x64->E_CALL_FROM_IAT(),lib,func);
                        free(lib);
                        free(func);
                        num++;
                    }    
                }
                else if ((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.index == X86_REG_INVALID)) {
                    if (arch->Get_Address_At(insn->detail->x86.operands[0].mem.disp,&addr,insn->detail->x86.addr_size*8)) {
                        if (!Excluded(addr)) {
                            if (ValidCode(addr)) {
                                name = c->GetFunctionName(addr);
                                if (name != NULL) {
                                    PrintLine(insn,1,lang_x64->E_FUNC_NAME(),name,raddr);
                                    free(name);   
                                }
                                else {
                                    PrintLine(insn,1,lang_x64->E_FUNC_ADDR(),addr,raddr);
                                }
                                num++;
                            }
                        }
                        else {
                            PrintLine(insn,0,lang_x64->E_SPACE());
                            num++;
                        }
                    }
                }
                else {
                    sprintf(buffer,"anoncall(op0,%llu)",raddr);
                    reg0 = lang_x64->Translate(handle,buffer,insn,true);
                    PrintLine(insn,1,reg0);
                    free(reg0);
                    num++;
                }
            }
            else {
                // Register
                sprintf(buffer,"anoncall(op0,%llu)",raddr);
                reg0 = lang_x64->Translate(handle,buffer,insn,true);
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            */
            raddr = insn->address+insn->size;
            addr = 0;
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                // call            0x180002240
                addr = insn->detail->x86.operands[0].imm;
            }
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                // Memory
                if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                    addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;  
                }
                else if ((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.index == X86_REG_INVALID)) {
                    if (!arch->Get_Address_At(insn->detail->x86.operands[0].mem.disp,&addr,insn->detail->x86.addr_size*8)) {
                        addr = 0;
                    }
                }
            }
            if (addr != 0) {
                if (!Excluded(addr)) {
                    if (arch->IsImportFunction(addr,&lib,&func)) {
                        PrintLine(insn,1,lang_x64->E_CALL_FROM_IAT(),lib,func);
                        free(lib);
                        free(func);
                        num++;
                    } 
                    else if (ValidCode(addr)) {
                        name = c->GetFunctionName(addr);
                        if (name != NULL) {
                            PrintLine(insn,1,lang_x64->E_FUNC_NAME(),name,raddr);
                            free(name);   
                        }
                        else {
                            PrintLine(insn,1,lang_x64->E_FUNC_ADDR(),addr,raddr);
                        }
                        num++;
                    }
                    else {
                        // Anon
                        sprintf(buffer,"anoncall(op0,%llu)",raddr);
                        reg0 = lang_x64->Translate(handle,buffer,insn,true);
                        PrintLine(insn,1,reg0);
                        free(reg0);
                        num++; 
                    }
                }
                else {
                    // Excluded
                    PrintLine(insn,0,lang_x64->E_SPACE());
                    num++;
                }             
            }
            else {
                // Anon
                sprintf(buffer,"anoncall(op0,%llu)",raddr);
                reg0 = lang_x64->Translate(handle,buffer,insn,true);
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            break;
        case X86_INS_SCASB:
        case X86_INS_SCASW:
        case X86_INS_SCASD:
        case X86_INS_SCASQ:
            strcpy (buffer, "cf(op1 > op0);"
                            "zf(op0 == op1);"
                            "sub_of(bits,sop0,sop1);"
                            "sf((sop0-sop1) < 0);"
                            "if get_df() then "
                                "rdi = rdi - bytes0 "
                            "else "
                                "rdi = rdi + bytes0 "
                            "fi");
            reg1 = AddREPX(buffer,insn);
            reg0 = lang_x64->Translate(handle,reg1,insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            free(reg1);
            num++;
            /*
            if (insn->detail->x86.prefix[0] == X86_PREFIX_REPNE) {
                reg0 = lang_x64->Translate(handle,  "while rcx != 0 do "
                                                        "cf(op1 > op0);"
                                                        "zf(op0 == op1);"
                                                        "sub_of(bits,sop0,sop1);"
                                                        "sf((sop0-sop1) < 0);"
                                                        "if get_df() then "
                                                            "rdi = rdi - bytes0 "
                                                        "else "
                                                            "rdi = rdi + bytes0 "
                                                        "fi;"
                                                        "rcx = rcx - 1;"
                                                        "if get_zf() then break fi "
                                                    "endw",insn,true);
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            */
            break;
        case X86_INS_MOVSB:
        case X86_INS_MOVSW:
        case X86_INS_MOVSD:
        case X86_INS_MOVSQ:
            strcpy (buffer, "op0 = op1;"
                            "if get_df() then "
                                "rdi = rdi - bytes0; rsi = rsi - bytes0 "
                            "else "
                                "rdi = rdi + bytes0; rsi = rsi + bytes0 "
                            "fi");
            reg1 = AddREPX(buffer,insn);
            reg0 = lang_x64->Translate(handle,reg1,insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            free(reg1);
            num++;
            /*
            if (insn->detail->x86.prefix[0]== X86_PREFIX_REP) {
                reg0 = lang_x64->Translate(handle,  "while rcx != 0 do "
                                                        "op0 = op1;"
                                                        "if get_df() then "
                                                            "rdi = rdi - bytes0; rsi = rsi - bytes0 "
                                                        "else "
                                                            "rdi = rdi + bytes0; rsi = rsi + bytes0 "
                                                        "fi;"
                                                        "rcx = rcx - 1"
                                                    "endw",insn,true);
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            */
            break;
        case X86_INS_CMPSB:
        case X86_INS_CMPSW:
        case X86_INS_CMPSD:
        case X86_INS_CMPSQ:
            strcpy (buffer, "cf(op1 > op0);"
                            "zf(op0 == op1);"
                            "sub_of(bits,sop0,sop1);"
                            "sf((sop0-sop1) < 0);"
                            "if get_df() then "
                                "rdi = rdi - bytes0; rsi = rsi - bytes0 "
                            "else "
                                "rdi = rdi + bytes0; rsi = rsi + bytes0 "
                            "fi");
            reg1 = AddREPX(buffer,insn);
            reg0 = lang_x64->Translate(handle,reg1,insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            free(reg1);
            num++;
            break;
        case X86_INS_STOSB:
        case X86_INS_STOSW:
        case X86_INS_STOSD:
        case X86_INS_STOSQ:
            strcpy (buffer, "op0 = op1;"
                            "if get_df() then "
                                "rdi = rdi - bytes0 "
                            "else "
                                "rdi = rdi + bytes0 "
                            "fi");
            reg1 = AddREPX(buffer,insn);
            reg0 = lang_x64->Translate(handle,reg1,insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            free(reg1);
            num++;
            break;
        case X86_INS_LODSB:
        case X86_INS_LODSW:
        case X86_INS_LODSD:
        case X86_INS_LODSQ:
            strcpy (buffer, "op0 = op1;"
                            "if get_df() then "
                                "rsi = rsi - bytes0 "
                            "else "
                                "rsi = rsi + bytes0 "
                            "fi");
            reg1 = AddREPX(buffer,insn);
            reg0 = lang_x64->Translate(handle,reg1,insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            free(reg1);
            num++;
            break;
        case X86_INS_STC:
            reg0 = lang_x64->Translate(handle,"cf(true)",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_CLC:
            reg0 = lang_x64->Translate(handle,"cf(false)",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_STD:
            reg0 = lang_x64->Translate(handle,"df(true)",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_CLD:
            reg0 = lang_x64->Translate(handle,"df(false)",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;     
        case X86_INS_FILD:
            reg0 = lang_x64->Translate(handle,"pushfpu(op0)",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_FLD:
            if (IsSTX(insn->detail->x86.operands[0].reg)) {
                reg0 = lang_x64->Translate(handle,  "pushfpu(op0)",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "if bits0 == 32 then "
                                                        "pushfpu(utof(op0)) "
                                                    "fi;"
                                                    "if bits0 == 64 then "
                                                        "pushfpu(utod(op0)) "
                                                    "fi;"
                                                    "if bits0 > 64 then "
                                                        "pushfpu(utol(op0)) "
                                                    "fi",insn,true);
            }
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_FIST:
            reg0 = lang_x64->Translate(handle,"sop0 = st0",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_FISTP:
            reg0 = lang_x64->Translate(handle,"sop0 = popfpu()",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_FSTP:
            if (IsSTX(insn->detail->x86.operands[0].reg)) {
                reg0 = lang_x64->Translate(handle,  "op0 = popfpu()",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "if bits0 == 32 then "
                                                        "op0 = ftou(popfpu()) "
                                                    "fi;"
                                                    "if bits0 == 64 then "
                                                        "op0 = dtou(popfpu()) "
                                                    "fi;"
                                                    "if bits0 > 64 then "
                                                        "op0 = ltou(popfpu()) "
                                                    "fi",insn,true);
            }
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_FST:
            if (IsSTX(insn->detail->x86.operands[0].reg)) {
                reg0 = lang_x64->Translate(handle,  "op0 = st0",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,  "if bits0 == 32 then "
                                                        "op0 = ftou(st0) "
                                                    "fi;"
                                                    "if bits0 == 64 then "
                                                        "op0 = dtou(st0) "
                                                    "fi;"
                                                    "if bits0 > 64 then "
                                                        "op0 = ltou(st0)  "
                                                    "fi",insn,true);
            }
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_FLDZ:
            reg0 = lang_x64->Translate(handle,"pushfpu(0)",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_FADD:
            if (insn->detail->x86.op_count == 1) {
                if (IsSTX(insn->detail->x86.operands[0].reg)) {
                    reg0 = lang_x64->Translate(handle,  "st0 = st0 + op0",insn,true);
                }
                else {
                    reg0 = lang_x64->Translate(handle,  "if bits0 == 32 then "
                                                            "st0 = st0 + utof(op0) "
                                                        "fi;"
                                                        "if bits0 == 64 then "
                                                            "st0 = st0 + utod(op0) "
                                                        "fi;"
                                                        "if bits0 > 64 then "
                                                            "st0 = st0 + utol(op0) "
                                                        "fi",insn,true);
                }                                        
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            break;
        case X86_INS_FSUB:
            if (insn->detail->x86.op_count == 1) {
                if (IsSTX(insn->detail->x86.operands[0].reg)) {
                    reg0 = lang_x64->Translate(handle,  "st0 = st0 - op0",insn,true);
                }
                else {
                    reg0 = lang_x64->Translate(handle,  "if bits0 == 32 then "
                                                            "st0 = st0 - utof(op0) "
                                                        "fi;"
                                                        "if bits0 == 64 then "
                                                            "st0 = st0 - utod(op0) "
                                                        "fi;"
                                                        "if bits0 > 64 then "
                                                            "st0 = st0 - utol(op0) "
                                                        "fi",insn,true);
                }
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            break;
        case X86_INS_FDIV:
            if (insn->detail->x86.op_count == 1) {
                if (IsSTX(insn->detail->x86.operands[0].reg)) {
                    reg0 = lang_x64->Translate(handle,  "st0 = st0 / op0",insn,true);
                }
                else {
                    reg0 = lang_x64->Translate(handle,  "if bits0 == 32 then "
                                                            "st0 = st0 / utof(op0) "
                                                        "fi;"
                                                        "if bits0 == 64 then "
                                                            "st0 = st0 / utod(op0) "
                                                        "fi;"
                                                        "if bits0 > 64 then "
                                                            "st0 = st0 / utol(op0) "
                                                        "fi",insn,true);
                }                                        
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            break;
        case X86_INS_FMUL:
            if (insn->detail->x86.op_count == 1) {
                if (IsSTX(insn->detail->x86.operands[0].reg)) {
                    reg0 = lang_x64->Translate(handle,  "st0 = st0 * op0",insn,true);
                }
                else {
                    reg0 = lang_x64->Translate(handle,  "if bits0 == 32 then "
                                                            "st0 = st0 * utof(op0) "
                                                        "fi;"
                                                        "if bits0 == 64 then "
                                                            "st0 = st0 * utod(op0) "
                                                        "fi;"
                                                        "if bits0 > 64 then "
                                                            "st0 = st0 * utol(op0) "
                                                        "fi",insn,true);
                }
                PrintLine(insn,1,reg0);
                free(reg0);
                num++;
            }
            break;
        case X86_INS_FNSTCW:
            // TODO: ALL
            reg0 = lang_x64->Translate(handle,"op0 = 0",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_FINCSTP:
            // TODO: ALL
            reg0 = lang_x64->Translate(handle,"popfpu()",insn,true);
            PrintLine(insn,1,reg0);
            free(reg0);
            num++;
            break;
        case X86_INS_FFREE:
        case X86_INS_FLDCW:
            // TODO: ALL
            if (insn->detail->x86.op_count == 1) {
                PrintLine(insn,1,"");
                num++;
            }
            break;
        default:
            break;
    }
    return (num);
}

int Base_x64::PrintInst(Code *c,struct _subcode *sc,int num) {
int n,ret;
char subname[1024];
char params[1024];
char buffer[1024];
cs_insn *insn;

    ret = PrintExtra(c,sc,num);
    if (ret > num) {
        // Done, next instruction
        return (ret);
    }
    // Not implemented
    subname[0] = 0;
    params[0] = 0;
    insn = &sc->insn[num];
    for (n=0;n<insn->detail->x86.op_count;n++) {
        switch (insn->detail->x86.operands[n].type) {
            case X86_OP_REG:
                strcat(subname,"r");
                sprintf(buffer,lang_x64->OP_REG(),reg_name(handle,insn->detail->x86.operands[n].reg));
                strcat(params,buffer);
                break;
            case X86_OP_IMM:
                strcat(subname,"i");
                sprintf(buffer,lang_x64->OP_IMM(),insn->detail->x86.operands[n].imm);
                strcat(params,buffer);
                break;
            case X86_OP_MEM:
                strcat(subname,"m");
                sprintf(buffer,lang_x64->OP_MEM(),reg_name(handle,insn->detail->x86.operands[n].mem.base),
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
        PrintLine(insn,1,lang_x64->OP_SUBNAME(),subname,insn->mnemonic,params);
    } else {
        PrintLine(insn,1,lang_x64->OP_ALONE(),insn->mnemonic);
    }
    return (num+1);
}

void Base_x64::PrintSubCode(Code *c,int num) {
struct _subcode *sc;
int id;
char *name;

    id = c->subcodes[num].id;
    name = c->GetFunctionName(c->subcodes[num].first);
    if (name != NULL) {
        //printf(C_FUNC_HEADER_NAME,name,c->subcodes[num].first);
        lang_x64->PrintFuncHeaderName(c,num,name);
        free(name);
    }
    else {
        //printf(C_FUNC_HEADER_ADDR,c->subcodes[num].first);
        lang_x64->PrintFuncHeaderAddr(c,num);
    }
    if (lstack) {
        int inputs = 0;
        printf("%s inputs:\n",lang_x64->COMM());
        // Call registers
        for (int n=0;n<X86_REG_ENDING;n++) {
            //if ((n != X86_REG_ESP) && (n != X86_REG_RSP)) {
                if (c->subcodes[num].regs[n] == REG_USED) {
                    lang_x64->PrintF("%s %s\n",lang_x64->COMM(),lang_x64->reg_name(handle,n));
                    inputs++;
                }
            //}
        }
        // Call Stack
        if (c->subcodes[num].ret_bytes) {
            int b = (arch->Is32()?4:8);
            char *sp = lang_x64->reg_name(handle,(arch->Is32()?X86_REG_EBP:X86_REG_RBP));
            for (int n=0;n<(c->subcodes[num].ret_bytes/b);n++) {
                lang_x64->PrintF("%s [%s+%li]\n",lang_x64->COMM(),sp,(n+1)*b);
                inputs++;
            }
            free(sp);
        }
        if (inputs == 0) {
            lang_x64->PrintF("%s none.\n",lang_x64->COMM());
        }
    }
    // Anno jmp var
    //if (c->subcodes[num].anonjmp && c->subcodes[num].l_count) {
    if (true) {
        // Always print anon vars
        lang_x64->PrintAnonJmpVar();
    }
    for (int m=0;m<c->subcod_count;m++) {
        sc = &c->subcodes[m];
        if ((sc->id == id) || (sc->parent == id)) {
            if (sc->parent != SUBCODE_TOP) {
                lang_x64->PrintSubCodeSep();
            }
            for (int n=0;n<sc->count;) {
                if (sc->insn[n].address > sc->last) {
                    break;
                }
                /*
                if (std::find(c->labels.begin(), c->labels.end(), sc->insn[n].address) != c->labels.end()) {
                    printf(lang_x64->E_LABEL,sc->insn[n].address);
                }
                */
                PrintLabel(c,sc,sc->insn[n].address);
                n = PrintInst(c,sc,n);
            }
        }
    }
    //printf(C_FUNC_FOOTER);
    // Anon jmps
    //if (c->subcodes[num].anonjmp && c->subcodes[num].l_count) {
    if (true) {
        // Always print anon jumps
        lang_x64->PrintSubCodeSep();
        lang_x64->PrintF(lang_x64->E_LABEL_ANON());
        for (int n=0;n<c->subcodes[num].l_count;n++) {
            uint64_t addr = c->subcodes[num].labels[n];
            name = GetLabel(addr);
            lang_x64->PrintAnonJmpCall(addr,name);
            /*
            printf("    if (anon == 0x%llx) {\n",addr);
            printf("        goto %s;\n",name);
            printf("    }\n");
            */
            free(name);
        }
        lang_x64->PrintAnonJmpEnd();
    }
    lang_x64->PrintFuncFooter(c,num);
}


void Base_x64::PrintCode(Code *c) {
struct _Section *sections;
int count,n;
char *name;

    // Add Stack memory
    c->AddSubMem(STACK_ADDR,NULL,STACK_SIZE);
    // Header
    lang_x64->PrintHeader(c);
    // Code
    lang_x64->PrintF("\n");
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            if (c->subcodes[n].first == 0x53610) {
                count = 0;  // test
            }
            PrintSubCode(c,n);
        }
    }
    // Anonymous Calls
    lang_x64->PrintAnonCalls(c);
    lang_x64->PrintF("\n");
    // Main function
    lang_x64->PrintMainOpen(c);
    // Load Errors
    lang_x64->PrintLoadError("OP",1,"Opcode not implemented, could be an error but if not, you must wait the next version.");
    lang_x64->PrintLoadError("GETMEM",2,"Read memory address not found, if it's correct, use the '-u' option to add the address.");
    lang_x64->PrintLoadError("SETMEM",3,"Set memory address not found, if it's correct, use the '-u' option to add the address.");
    lang_x64->PrintLoadError("CALL",4,"Call address not found, if it's correct, use the '-i' option to add the calling address.");
    lang_x64->PrintLoadError("JMP",5,"Jmp address not found.");
    lang_x64->PrintLoadError("IAT",6,"Call to an external library function not implemented.");
    lang_x64->PrintLoadError("DEFAULT",255,"Default error, error description not implemented yet.");
    // Load Submemories
    for (int n=0;n<c->submem_count;n++) {
        lang_x64->PrintSubMem(c,n);
    }
    if (loadm) {
        //Load Memory form file
        sections = arch->GetSections(&count);
        if (sections != NULL) {
            for (n=0;n<count;n++) {
                PrintLine(NULL,1,lang_x64->E_LOAD_MEM(),arch->GetFileName(),sections[n].d_Offset,sections[n].d_Size,sections[n].v_Address,sections[n].v_Size);
            }
            free(sections);
        }
    }
    lang_x64->PrintF(lang_x64->E_STACK_INIT(),lang_x64->reg_name(handle,X86_REG_RSP),STACK_ADDR+(STACK_SIZE/2),
                                lang_x64->reg_name(handle,X86_REG_RBP),
                                lang_x64->reg_name(handle,X86_REG_RSP));
    //printf(C_FOOTER_2,c->ep);
    IsNamedFunction(c->ep,&name);
    lang_x64->PrintMainClose(c,name);
    if (name != NULL) free(name);
}

#define SETAN1(R)   if (((reg == X86_REG_##R##L) || (reg == X86_REG_##R##H)) && (p->regs[X86_REG_E##R##X] != REG_NONE)) return;\
                    if ((reg == X86_REG_E##R##X) && (p->regs[X86_REG_R##R##X] != REG_NONE)) return;

void Base_x64::SetAnalyzed(struct _subcode *p,int reg,int value) {
    //if (((reg == X86_REG_AL) || (reg == X86_REG_AH)) && (p->regs[X86_REG_EAX] != REG_NONE)) return;
    SETAN1(A);
    SETAN1(B);
    SETAN1(C);
    SETAN1(D);
    if ((reg != X86_REG_ESP) && (reg != X86_REG_RSP)) {
        if (p->regs[reg] == REG_NONE) {
            p->regs[reg] = value;
        }
    }
}

void Base_x64::AnalyzeUsed(struct _subcode *p,cs_insn *insn,int op) {
    if (insn->detail->x86.op_count >= (op+1)) {
        if (insn->detail->x86.operands[op].type == X86_OP_REG) {
            SetAnalyzed(p,insn->detail->x86.operands[op].reg,REG_USED);
            /*
            if (p->regs[insn->detail->x86.operands[op].reg] == REG_NONE) {
                //p->regs[insn->detail->x86.operands[op].reg] = REG_USED;
            }
            */
        }
        else if (insn->detail->x86.operands[op].type == X86_OP_MEM) {
            if (insn->detail->x86.operands[op].mem.base != X86_REG_INVALID) {
                /*
                if (p->regs[insn->detail->x86.operands[op].mem.base] == REG_NONE) {
                    p->regs[insn->detail->x86.operands[op].mem.base] = REG_USED;
                }
                */
                SetAnalyzed(p,insn->detail->x86.operands[op].mem.base,REG_USED);
            }
            if (insn->detail->x86.operands[op].mem.index != X86_REG_INVALID) {
                /*/
                if (p->regs[insn->detail->x86.operands[op].mem.index] == REG_NONE) {
                    p->regs[insn->detail->x86.operands[op].mem.index] = REG_USED;
                }
                */
                SetAnalyzed(p,insn->detail->x86.operands[op].mem.index,REG_USED);
            }
        }
    }
}

void Base_x64::AnalyzeUpdated(struct _subcode *p,cs_insn *insn,int op) {
    if (insn->detail->x86.op_count >= (op+1)) {
        if (insn->detail->x86.operands[op].type == X86_OP_REG) {
            /*
            if (p->regs[insn->detail->x86.operands[op].reg] == REG_NONE) {
                p->regs[insn->detail->x86.operands[op].reg] = REG_UPDATED;
            }
            */
            SetAnalyzed(p,insn->detail->x86.operands[op].reg,REG_UPDATED);
        }
    }
}

void Base_x64::AnalyzeSubCode(Code *c,int num) {
int n;
struct _subcode *sc,*p; 
cs_insn *insn;

    sc = &c->subcodes[num];
    if (sc->first == 0x4131e4) {
        n = 0;  // Test
    }
    p = c->GetParent(sc);
    for (n=0;n<sc->count;) {
        insn = &sc->insn[n];
        //printf("%s 0x%llx:\t%s\t\t%s\n", lang_x64->COMM(), insn->address, insn->mnemonic,insn->op_str);
        switch (sc->insn[n].id) {
            case X86_INS_MOV:
            case X86_INS_MOVZX:
            case X86_INS_POP:
                AnalyzeUsed(p,insn,1);
                AnalyzeUpdated(p,insn,0);
                break;
            case X86_INS_ADD:
            case X86_INS_ADC:
            case X86_INS_SUB:
            case X86_INS_SBB:
            case X86_INS_AND:
            case X86_INS_OR:
            case X86_INS_TEST:
            case X86_INS_CMP:
                AnalyzeUsed(p,insn,1);
                AnalyzeUsed(p,insn,0);
                break;
            case X86_INS_XOR:
                if ((insn->detail->x86.operands[0].type == X86_OP_REG) 
                        && (insn->detail->x86.operands[1].type == X86_OP_REG) 
                        && (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg)) {
                    AnalyzeUpdated(p,insn,0);
                }
                else {
                    AnalyzeUsed(p,insn,1);
                    AnalyzeUsed(p,insn,0);
                }
                break;
        }
        if (insn->address == sc->last) {
            // last instruction
            break;
        }
        n++;
    }
    if (sc->parent == SUBCODE_TOP) {
        for (n=0;n<c->subcod_count;n++) {
            if ((c->subcodes[n].parent != SUBCODE_TOP) && (c->subcodes[n].parent == sc->id)) {
                AnalyzeSubCode(c,n);
            }
        }
    }
}

void Base_x64::AnalyzeCode(Code *c) {
int n,m;

    for (n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            for (m=0;m<X86_REG_ENDING;m++) {
                c->subcodes[n].regs[m] = REG_NONE;
            }
            AnalyzeSubCode(c,n);
        }
    }
}

Base_x64::Base_x64(Archive *a,Language *l) : Butcher(a,l) {
    lang_x64 = (Lang_x64 *)l;
}