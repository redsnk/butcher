#include "base_x64.hpp"
#include "../files/pe.hpp"
#include <capstone/capstone.h>

const char *reg_name(csh handle,int id_reg) {
    if (id_reg == X86_REG_INVALID) {
        return ("");
    }
    return (cs_reg_name(handle,id_reg));
}
/*
uint32_t memto32 (uint8_t *mem) {
uint32_t v;

    v = (((uint32_t ) mem[3]) << 24) | 
        (((uint32_t ) mem[2]) << 16) | 
        (((uint32_t ) mem[1]) << 8) | 
        ((uint32_t ) mem[0]);
    return(v);
}

uint64_t memto64 (uint8_t *mem) {
uint64_t v;

    v = (((uint64_t ) mem[7]) << 56) | 
        (((uint64_t ) mem[6]) << 48) | 
        (((uint64_t ) mem[5]) << 40) | 
        (((uint64_t ) mem[4]) << 32) | 
        (((uint64_t ) mem[3]) << 24) | 
        (((uint64_t ) mem[2]) << 16) | 
        (((uint64_t ) mem[1]) << 8) | 
        ((uint64_t ) mem[0]);
    return(v);
}
*/

/*
const char *get_reg64(const char *reg) {
// rax, eax, ax, ah, al
#define STRCMPREG(r)	!strcmp(reg,"r"#r"x") || !strcmp(reg,"e"#r"x") || !strcmp(reg,#r"x") || !strcmp(reg,#r"h") || !strcmp(reg,#r"l")
// rsp, esp, sp, spl
#define STRCMPREG2(r)    !strcmp(reg,"r"#r) || !strcmp(reg,"e"#r) || !strcmp(reg,#r) || !strcmp(reg,#r"l")
// r8, r8d, r8w, r8b
#define STRCMPREG3(r)    !strcmp(reg,#r) || !strcmp(reg,#r"d") || !strcmp(reg,#r"w") || !strcmp(reg,#r"b")

	if (STRCMPREG(a)) {
		return ("rax");
	}
	else if (STRCMPREG(b)) {
		return ("rbx");
	}
	else if (STRCMPREG(c)) {
		return ("rcx");
	}
	else if (STRCMPREG(d)) {
        return ("rdx");
    }
	else if (STRCMPREG2(sp)) {
        return ("rsp");
	}
	else if (STRCMPREG2(bp)) {
        return ("rbp");
    }
    else if (STRCMPREG2(si)) {
        return ("rsi");
    }
    else if (STRCMPREG2(di)) {
        return ("rdi");
    }
    else if (STRCMPREG3(r8)) {
        return ("r8");
    }
    else if (STRCMPREG3(r9)) {
        return ("r9");
    }
    else if (STRCMPREG3(r10)) {
        return ("r10");
    }
    else if (STRCMPREG3(r11)) {
        return ("r11");
    }
    else if (STRCMPREG3(r12)) {
        return ("r12");
    }
    else if (STRCMPREG3(r13)) {
        return ("r13");
    }
    else if (STRCMPREG3(r14)) {
        return ("r14");
    }
    else if (STRCMPREG3(r15)) {
        return ("r15");
    }
	return (NULL);
}

#define MAX_REG     (16)

void reg_cpu(csh handle,int id_reg,char *buffer) {
char f,l;
const char *reg,*r64;
int len;

    // rax, eax, ax, ah, al
    // rsp, esp, sp, spl
    // r8, r8d, r8w, r8b
    reg = reg_name(handle,id_reg);
    r64 = get_reg64(reg);
    len = strlen(reg);
	f = reg[0];
	l = reg[len-1];
	if ((f == 'r') && (l != 'd') && (l != 'w') && (l != 'b')) {
        // 64
		sprintf(buffer,"cpu->%s.r64",r64);
    }
    else if ((f == 'e') || (l == 'd')) {
        // 32
        sprintf(buffer,"cpu->%s.r32.l",r64);
    }
    else if (l == 'h') {
        // 8 (h)
        sprintf (buffer,"cpu->%s.r8.h",r64);
    }
    else if (l == 'l') { 
        // 8 (l)
        sprintf (buffer,"cpu->%s.r8.l",r64);
    }
    else {
        // 16
        sprintf (buffer,"cpu->%s.r16.l",r64);
    }
}
*/

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

int ClearFlagInst(cs_insn *insn) {
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
            return (true);
    }
    return (false);
}

int UseFlagInst(cs_insn *insn) {
    if (JccInst(insn) || SetInst(insn)) {
        return (true);
    }
    switch (insn->id) {
        case X86_INS_ADC:
        case X86_INS_SBB:
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
        if (ClearFlagInst(insn)) {
            return (true);
        }
        num++;
    }
    return (true);
}

int IsRIP(int id) {
    return((id == X86_REG_EIP)||(id == X86_REG_RIP));
}

int MemUtil(uint8_t *mem,int read) {
int n,c;

    if (read >= 8) {
        // Unicode
        n = 0;
        do {
            if ((mem[n] != 0) && (mem[n+1] == 0)) {
                n += 2;
            } 
            else {
                break;
            }  
        }
        while (n < (read-1));
        c = (n/2);
        if ((c >= 4) && (mem[n] == 0) && (mem[n+1] == 0)) {
            // 4+ chars
            return ((c+1)*2);
        }
    }
    return (0);
}

// ---------------------------------------------------------------------------------------------------------

cs_err Base_x64::Cs_open(void) {
    return(cs_open(CS_ARCH_X86, arch->Is32()?CS_MODE_32:CS_MODE_64, &handle));
}

int Base_x64::IsRet(cs_insn *insn) {
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
                if ((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                    (insn->detail->x86.operands[0].mem.index == X86_REG_INVALID)) {
                    /*
                    if (arch->ValidMemory(insn->detail->x86.operands[0].mem.disp)) {
                        // 
                        *addr = insn->detail->x86.operands[0].mem.disp;
                        return (true);
                    }
                    */
                    if (arch->Get_Address_At(insn->detail->x86.operands[0].mem.disp,addr,insn->detail->x86.addr_size*8)) {
                        if (arch->ValidMemory(*addr)) {
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
    else if ((insn[n].size == 2) && !insn[n].bytes[0] && !insn[n].bytes[1]) {
        // 0000           add byte [eax], al
        return (true);
    }
    return (false);
}

/*
int Base_x64::IsJmpIAT(cs_insn *insn) {
uint64_t addr;
char *lib,*func;

    if (insn->id == X86_INS_JMP) {
        if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
            if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;
                if (arch->IsImportFunction(addr,&lib,&func)) {
                    free(lib);
                    free(func);
                    return (true);
                }    
            }
            else if((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                    (insn->detail->x86.operands[0].mem.index == X86_REG_INVALID) && 
                    (insn->detail->x86.operands[0].mem.disp)) {
                addr = insn->detail->x86.operands[0].mem.disp;
                if (arch->IsImportFunction(addr,&lib,&func)) {
                    free(lib);
                    free(func);
                    return (true);
                }
            }
        }
    }
    return (false);
}
*/

#define MAX_JMPS      (1024*10)

int Base_x64::IsJmp(cs_insn *in, int num, uint64_t *addr[],int *count) {
int n,b,c;
uint8_t *mem;
uint64_t read,a,d;
cs_insn *insn;

    insn = &in[num];
    c = 0;
    if (insn->id == X86_INS_JMP) {
        if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
            *addr = (uint64_t *) malloc(sizeof(uint64_t));
            (*addr)[c++] = insn->detail->x86.operands[0].imm;
        }
        else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
            if((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                            (insn->detail->x86.operands[0].mem.index != X86_REG_INVALID) && 
                            (insn->detail->x86.operands[0].mem.scale > 1) && (insn->detail->x86.operands[0].mem.disp)) {
                for (n=0;n<MAX_JMPS;n++) {
                    a = insn->detail->x86.operands[0].mem.disp + (insn->detail->x86.operands[0].mem.scale * n);
                    b = insn->detail->x86.addr_size;
                    if (arch->Get_Address_At(a,&d,b*8)) {
                        if (arch->ValidMemory(d)) {
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
            else {
               // Undefined jmp
                *addr = (uint64_t *) malloc(sizeof(uint64_t));
                (*addr)[c++] = UNDEF_ADDR_JMP; 
            }
        }
        else {
            // Undefined jmp X86_OP_REG
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
                            if (arch->ValidMemory(in[n].detail->x86.operands[0].imm)) {
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

#define MAX_SUBMEM  (1024*5)

int Base_x64::IsSubMem(cs_insn *insn, uint64_t *addr, uint8_t **mem, int *count) {
uint64_t read;
int n;

    if (insn->id == X86_INS_MOV) {
        if (insn->detail->x86.operands[0].type == X86_OP_REG) {
            if (insn->detail->x86.operands[1].type == X86_OP_IMM) {
                *addr = insn->detail->x86.operands[1].imm;
                *mem = arch->GetMemory(*addr,MAX_SUBMEM,&read);
                if (*mem != NULL) {
                    *count = MemUtil(*mem,read);
                    if (*count) {
                        return (true);
                    }
                    free(*mem);
                }
            }
            else if (insn->detail->x86.operands[1].type == X86_OP_MEM) {
                if (IsRIP(insn->detail->x86.operands[1].mem.base)) {
                    // mov rax, qword ptr [**rip** + 0x1dc97]
                    *addr = insn->address + insn->size + insn->detail->x86.operands[1].mem.disp;
                    n = insn->detail->x86.addr_size;
                    *mem = arch->GetMemory(*addr,n,&read);
                    if (*mem != NULL) {
                        if (read == n) {
                            *count = read;
                            return (true);
                        }
                        free(mem);
                    }
                }
            }
        }
    }
    return (false);
}

// ---------------------------------------------------------------------------------------------------------

void Base_x64::PrintLine(cs_insn *insn,int indent,const char *format,...) {
char buffer[1024];
char out[1024];

    va_list argptr;
    va_start(argptr, format);
    vsprintf(buffer,format, argptr);
    va_end(argptr);
    out[0] = 0;
    for (int n=0;n<indent;n++) {
        strcat (out,lang_x64->INDENT);
    }
    strcat (out,buffer);
    char *p = out;
    
    for (int n=0;n<strlen(out);n++) {
        if (out[n] == '\n') {
            p = out+n+1;
        }
    }
    
    //while (strlen(buffer) < lang_x64->COMM_SEP) strcat(buffer," ");
    if ((insn == NULL) || !lasm) {
        printf("%s\n", out);
    }
    else {
        while (strlen(p) < lang_x64->COMM_SEP) strcat(p," ");
        printf("%s%s 0x%llx:\t%s\t\t%s\n", out, lang_x64->COMM, insn->address, insn->mnemonic,insn->op_str);
    }
}

void Base_x64::PrintLabel(Code *c,uint64_t addr) {
char *name;

    if (std::find(c->labels.begin(), c->labels.end(), addr) != c->labels.end()) {
        // TODO: NamedFunction
        /*
        if (IsNamedFunction(addr,&name)) {
            printf(lang_x64->E_LABEL_NAME,name);
            free(name);
        }
        else {
        */
            printf(lang_x64->E_LABEL,addr);
        //}
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
uint64_t read,d;
char *reg0,*reg1,*mstr;
uint64_t addr;
char *lib,*func,*name;
uint8_t *mem;
int n,b,ldone;
int bits;

    insn = &sc->insn[num];
    bits = insn->detail->x86.addr_size*8;
    if (insn->address == 0x40c5cd) {
        // test
        bits = 0;
    }
    if ((sc->parent == SUBCODE_TOP) && !num) {
        // First instruction, push rip/eip
        reg0 = lang_x64->Translate(handle,".push(bits,0);",insn,true);
        if (reg0 != NULL) {
            PrintLine(insn,0,reg0);
            free(reg0);
        }
    }
    switch (insn->id) {
        case X86_INS_NOP:
            PrintLine(insn,1,"");
            num++;
            break;
        case X86_INS_RET:
            // ret
            reg0 = lang_x64->Translate(handle,".pop(bits);",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                free(reg0);
            }
            if (insn->detail->x86.op_count > 0) {
                reg0 = lang_x64->Translate(handle,".rsp = rsp + op0;",insn,true);
                if (reg0 != NULL) {
                    PrintLine(insn,0,reg0);
                    free(reg0);
                }
            }
            PrintLine(insn,1,lang_x64->E_RETURN);
            num++;
            break;
        case X86_INS_JMP:
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                PrintLine(insn,1,lang_x64->E_GOTO,insn->detail->x86.operands[0].imm);
                num++;
            }
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                    addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;
                    if (arch->IsImportFunction(addr,&lib,&func)) {
                        PrintLine(insn,1,lang_x64->E_JMP_FROM_IAT,lib,func);
                        free(lib);
                        free(func);
                        num++;
                    }    
                }
                else if((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.index == X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.disp)) {
                    addr = insn->detail->x86.operands[0].mem.disp;
                    if (arch->IsImportFunction(addr,&lib,&func)) {
                        PrintLine(insn,1,lang_x64->E_JMP_FROM_IAT,lib,func);
                        free(lib);
                        free(func);
                        num++;
                    }
                }
                else if((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.index != X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.scale > 1) && (insn->detail->x86.operands[0].mem.disp)) {
                    // jmp             dword ptr [eax*4 + 0x44fcb89]
                    //PrintLine(insn,1,"switch(%s) {",lang_x64->reg_name(handle,insn->detail->x86.operands[0].mem.index));
                    reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].mem.index);
                    for (n=0;n<MAX_JMPS;n++) {
                        addr = insn->detail->x86.operands[0].mem.disp + (insn->detail->x86.operands[0].mem.scale * n);
                        b = insn->detail->x86.addr_size;
                        if (arch->Get_Address_At(addr,&d,b*8)) {
                            if (arch->ValidMemory(d)) {
                                PrintLine(insn,1,(n==0)?lang_x64->E_IF_R_EQ_I:lang_x64->E_ELIF_R_EQ_I,reg0,n);
                                //PrintLine(insn,2,"case %i:",n);
                                PrintLine(insn,2,lang_x64->E_GOTO,d);
                                PrintLine(insn,1,lang_x64->E_ENDIF);
                            }
                            else {
                                break;
                            }
                        }
                        else {
                            break;
                        }
                        /*
                        mem = arch->GetMemory(addr,b,&read);
                        if ((mem != NULL) && (read == b)) {
                            if (b == 4) {
                                addr = memto32(mem);
                            }
                            else if (b == 8) {
                                addr = memto64(mem);
                            }
                            free(mem);
                            if (arch->ValidMemory(addr)) {
                                // Valid
                                PrintLine(insn,1,(n==0)?lang_x64->E_IF_R_EQ_I:lang_x64->E_ELIF_R_EQ_I,reg0,n);
                                //PrintLine(insn,2,"case %i:",n);
                                PrintLine(insn,2,lang_x64->E_GOTO,addr);
                                PrintLine(insn,1,lang_x64->E_ENDIF);
                            }
                            else {
                                break;
                            }
                        }
                        else {
                            break;
                        }
                        */
                    }
                    free(reg0);
                    //PrintLine(insn,1,lang_x64->E_ENDIF);
                    num++;
                }
            }
            break;
        case X86_INS_JE:
            // (ZF=1)
            reg0 = lang_x64->Translate(handle,"get_zf();",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JNE:
            // (ZF=0)
            //PrintLine(insn,1,lang_x64->E_JNE,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"!get_zf();",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JA:
            // (CF=0 and ZF=0)
            //PrintLine(insn,1,lang_x64->E_JA,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"(!get_cf()) & (!get_zf());",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JAE:
            // (CF=0)
            //PrintLine(insn,1,lang_x64->E_JAE,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"!get_cf();",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JL:
            // (SF!=OF)
            //PrintLine(insn,1,lang_x64->E_JL,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"get_sf() != get_of();",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JLE:
            // (ZF=1 or SF!=OF)
            //PrintLine(insn,1,lang_x64->E_JLE,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"get_zf() | (get_sf() != get_of());",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JG:
            // (ZF=0 and SF=OF)
            //PrintLine(insn,1,lang_x64->E_JGE,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"(!get_zf()) & (get_sf() == get_of());",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JGE:
            // (SF=OF)
            //PrintLine(insn,1,lang_x64->E_JGE,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"get_sf() == get_of();",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JO:
            // (OF=1)
            //PrintLine(insn,1,lang_x64->E_JO,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"get_of();",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JNO:
            // (OF=0)
            //PrintLine(insn,1,lang_x64->E_JNO,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"!get_of();",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JS:
            // (SF=1)
            //PrintLine(insn,1,lang_x64->E_JS,insn->detail->x86.operands[0].imm);
            reg0 = lang_x64->Translate(handle,"get_sf();",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_JNS:
            // (SF=0)
            reg0 = lang_x64->Translate(handle,"!get_sf();",insn,false);
            PrintLine(insn,1,lang_x64->E_JCC_GOTO,reg0,insn->detail->x86.operands[0].imm);
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
                reg0 = lang_x64->Translate(handle,".push(bits,0);",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,".push(bits,op0);",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
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
            reg0 = lang_x64->Translate(handle,".op0 = pop(bits);",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_ADD:
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,".op0 = op0 + op1;",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,".add_cf(bits,op0,op1);:.add_of(bits,sop0,sop1);:.op0 = op0 + op1;:.zf(op0 == 0);:.sf(sop0 < 0);",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_XADD:
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,".push(bits,op0);:.op0 = op0 + op1;:.op1 = pop(bits);",insn,true);
                if (reg0 != NULL) {
                    PrintLine(insn,0,reg0);
                    num++;
                    free(reg0);
                }
            }
            break;
        case X86_INS_ADC:
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,".op0 = op0 + op1 + num_cf();",insn,true);
                PrintLine(insn,0,reg0);
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
                reg0 = lang_x64->Translate(handle,".op0 = op0 + 1;",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,".add_cf(bits,op0,1);:.add_of(bits,sop0,1);:.op0 = op0 + 1;:.zf(op0 == 0);:.sf(sop0 < 0);",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SUB:
            /*
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                if (FlagsNotUsed(sc,num)) {
                    reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
                    reg1 = lang_x64->get_op_str(handle,insn->detail->x86.operands[1],bits,false);
                    PrintLine(insn,1,lang_x64->E_SUB_RR,reg0,reg0,reg1);
                    num++;
                    free(reg1);
                    free(reg0);
                }
            }
            */
            // The OF, SF, ZF, AF, CF, and PF flags
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,".op0 = op0 - op1;",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,".cf(op1 > op0);:.sub_of(bits,sop0,sop1);:.op0 = op0 - op1;:.zf(op0 == 0);:.sf(sop0 < 0);",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
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
                reg0 = lang_x64->Translate(handle,".op0 = op0 - 1;",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,".cf(op0 == 0);:.sub_of(bits,sop0,1);:.op0 = op0 - 1;:.zf(op0 == 0);:.sf(sop0 < 0);",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
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
                        if (FlagsNotUsed(sc,num+1)) {
                            // "if (%s > %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 > op1;",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLabel(c,next->address);
                            PrintLine(next,1,lang_x64->E_JCC_GOTO,reg0,next->detail->x86.operands[0].imm);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JGE:
                        // (SF=OF)
                        if (FlagsNotUsed(sc,num+1)) {
                            // "if (%s >= %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"sop0 >= sop1;",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLabel(c,next->address);
                            PrintLine(next,1,lang_x64->E_JCC_GOTO,reg0,next->detail->x86.operands[0].imm);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JE:
                        // (ZF=1)
                        if (FlagsNotUsed(sc,num+1)) {
                            // "if (%s == %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 == op1;",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLabel(c,next->address);
                            PrintLine(next,1,lang_x64->E_JCC_GOTO,reg0,next->detail->x86.operands[0].imm);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JNE:
                        // (ZF=0)
                        if (FlagsNotUsed(sc,num+1)) {
                            // "if (%s != %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 != op1;",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLabel(c,next->address);
                            PrintLine(next,1,lang_x64->E_JCC_GOTO,reg0,next->detail->x86.operands[0].imm);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JBE:
                        // (CF=1 or ZF=1)
                        if (FlagsNotUsed(sc,num+1)) {
                            // "if (%s <= %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 <= op1;",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLabel(c,next->address);
                            PrintLine(next,1,lang_x64->E_JCC_GOTO,reg0,next->detail->x86.operands[0].imm);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JB:
                        // (CF=1)
                        if (FlagsNotUsed(sc,num+1)) {
                            // "if (%s < %s) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"op0 < op1;",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLabel(c,next->address);
                            PrintLine(next,1,lang_x64->E_JCC_GOTO,reg0,next->detail->x86.operands[0].imm);
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
                    reg0 = lang_x64->Translate(handle,".cf(op1 > op0);:.sub_of(bits,sop0,sop1);:.zf((op0-op1) == 0);:.sf((sop0-sop1) < 0);",insn,true);
                }
                if (reg0 != NULL) {
                    PrintLine(insn,0,reg0);
                    num++;
                    free(reg0);
                }
            }
            /*
            if ((num+1) < sc->count) {
                cs_insn *next = &sc->insn[num+1];
                switch (next->id) {
                    case X86_INS_JA:
                        // (CF=0 and ZF=0)
                        if (FlagsNotUsed(sc,num+1)) {
                            reg0 = lang_x64->get_op_str(handle,insn->detail->x86.operands[0],bits,false);
                            reg1 = lang_x64->get_op_str(handle,insn->detail->x86.operands[1],bits,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLine(next,1,lang_x64->E_JA_RR_GOTO,reg0,reg1,next->detail->x86.operands[0].imm);
                            free(reg0);
                            free(reg1);
                            num += 2;
                        }
                        break;
                    case X86_INS_JGE:
                        // (SF=OF)
                        if (FlagsNotUsed(sc,num+1)) {
                            reg0 = lang_x64->get_op_str(handle,insn->detail->x86.operands[0],bits,true);
                            reg1 = lang_x64->get_op_str(handle,insn->detail->x86.operands[1],bits,true);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLine(next,1,lang_x64->E_JGE_RR_GOTO,reg0,reg1,next->detail->x86.operands[0].imm);
                            free(reg0);
                            free(reg1);
                            num += 2;
                        }
                        break;
                }
            }
            */
            break;
        case X86_INS_OR:
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,".op0 = op0 | op1;",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,".cf(false);:.of(false);:.op0 = op0 | op1;:.zf(op0 == 0);:.sf(sop0 < 0);",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;       
        case X86_INS_XOR:
            /*
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                if (FlagsNotUsed(sc,num)) {
                    reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
                    if ((insn->detail->x86.operands[1].type == X86_OP_REG) && (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg)) {
                        PrintLine(insn,1,lang_x64->E_XOR_R,reg0);   
                    }
                    else {
                        reg1 = lang_x64->get_op_str(handle,insn->detail->x86.operands[1],bits,false);
                        PrintLine(insn,1,lang_x64->E_XOR_RR,reg0,reg0,reg1);
                        free(reg1);
                    }
                    num++;
                    free(reg0);
                }
            }
            */
            if ((insn->detail->x86.operands[0].type == X86_OP_REG) && (insn->detail->x86.operands[1].type == X86_OP_REG) &&
                (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg) && FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,".op0 = 0;",insn,true);
            }
            else if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,".op0 = op0 ^ op1;",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,".cf(false);:.of(false);:.op0 = op0 ^ op1;:.zf(op0 == 0);:.sf(sop0 < 0);",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_NOT:
            reg0 = lang_x64->Translate(handle,".op0 = not(bits,op0);",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_NEG:
            //The CF flag set to 0 if the source operand is 0; otherwise it is set to 1. The OF, SF, ZF, AF, and PF flags are set according to the result.
            reg0 = lang_x64->Translate(handle,".cf(op0 != 0);:.op0 = neg(bits,op0);:.zf(op0 == 0);:.sf(sop0 < 0);",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
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
                        if (FlagsNotUsed(sc,num+1)) {
                            // "if (%s != 0) goto label_0x%llx;";
                            reg0 = lang_x64->Translate(handle,"(op0&op1) != 0;",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLabel(c,next->address);
                            PrintLine(next,1,lang_x64->E_JCC_GOTO,reg0,next->detail->x86.operands[0].imm);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JE:
                        // (ZF=1)
                        if (FlagsNotUsed(sc,num+1)) {
                            // "if (%s == 0) goto label_0x%llx;"
                            reg0 = lang_x64->Translate(handle,"(op0&op1) == 0;",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLabel(c,next->address);
                            PrintLine(next,1,lang_x64->E_JCC_GOTO,reg0,next->detail->x86.operands[0].imm);
                            free(reg0);
                            num += 2;
                            ldone = true;
                        }
                        break;
                    case X86_INS_JLE:
                        // (ZF=1 or SF!=OF)
                        if (FlagsNotUsed(sc,num+1)) {
                            // "if (%s <= 0) goto label_0x%llx;"
                            reg0 = lang_x64->Translate(handle,"(sop0&sop1) <= 0;",insn,false);
                            PrintLine(insn,0,lang_x64->E_SPACE);
                            PrintLabel(c,next->address);
                            PrintLine(next,1,lang_x64->E_JCC_GOTO,reg0,next->detail->x86.operands[0].imm);
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
                    reg0 = lang_x64->Translate(handle,".cf(false);:.of(false);:.zf((op0&op1) == 0);:.sf((sop0&sop1) < 0);",insn,true);
                }
                if (reg0 != NULL) {
                    PrintLine(insn,0,reg0);
                    num++;
                    free(reg0);
                }
            }
            /*
            if ((num+1) < sc->count) {
                cs_insn *next = &sc->insn[num+1];
                switch (next->id) {
                    case X86_INS_JNE:
                        // (ZF=0)
                        if (FlagsNotUsed(sc,num+1)) {
                            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                                reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
                                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                                    if (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg) {
                                        // test		eax, eax
                                        // jne		0x1800026ce
                                        PrintLine(insn,0,lang_x64->E_SPACE);
                                        PrintLine(next,1,lang_x64->E_JNE_R_GOTO,reg0,next->detail->x86.operands[0].imm);
                                        num += 2;

                                    }
                                }
                                free(reg0);
                            }
                        }
                        break;
                    case X86_INS_JE:
                        // (ZF=1)
                        if (FlagsNotUsed(sc,num+1)) {
                            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                                reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
                                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                                    if (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg) {
                                        // test		eax, eax
                                        // je		0x1800026ce
                                        PrintLine(insn,0,lang_x64->E_SPACE);
                                        PrintLine(next,1,lang_x64->E_JE_R_GOTO,reg0,next->detail->x86.operands[0].imm);
                                        num += 2;

                                    }
                                }
                                free(reg0);
                            }
                        }
                        break;
                    case X86_INS_JLE:
                        // (ZF=1 or SF!=OF).
                        if (FlagsNotUsed(sc,num+1)) {
                            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                                reg0 = lang_x64->s_reg_name(handle,insn->detail->x86.operands[0].reg);
                                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                                    if (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg) {
                                        // test		eax, eax
                                        // jle		0x1800026ce
                                        PrintLine(insn,0,lang_x64->E_SPACE);
                                        PrintLine(next,1,lang_x64->E_JLE_R_GOTO,reg0,next->detail->x86.operands[0].imm);
                                        num += 2;

                                    }
                                }
                                free(reg0);
                            }
                        }
                        break;
                }
            }
            */
            break;
        case X86_INS_AND:
            if (FlagsNotUsed(sc,num)) {
                reg0 = lang_x64->Translate(handle,".op0 = op0 & op1;",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,".cf(false);:.of(false);:.op0 = op0 & op1;:.zf(op0 == 0);:.sf(sop0 < 0);",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SHR:
            if (FlagsNotUsed(sc,num)) {
                //reg0 = lang_x64->Translate(handle,".op0 = op0 / pow(2,op1);",insn,true);
                reg0 = lang_x64->Translate(handle,".op0 = op0 >> op1;",insn,true);
            }
            else {
                reg0 = NULL;
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SHL:
            if (FlagsNotUsed(sc,num)) {
                //reg0 = lang_x64->Translate(handle,".op0 = op0 * pow(2,op1);",insn,true);
                reg0 = lang_x64->Translate(handle,".op0 = op0 << op1;",insn,true);
            }
            else {
                reg0 = NULL;
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_IMUL:
            switch (insn->detail->x86.op_count) {
                case 1:
                    reg0 = lang_x64->Translate(handle,".s_rax = s_rax * sop0;",insn,true);
                    if (reg0 != NULL) {
                        PrintLine(insn,0,reg0);
                        num++;
                        free(reg0);
                    }
                    break;
                case 2:
                    reg0 = lang_x64->Translate(handle,".sop0 = sop0 * sop1;",insn,true);
                    if (reg0 != NULL) {
                        PrintLine(insn,0,reg0);
                        num++;
                        free(reg0);
                    }
                    break;
                case 3:
                    reg0 = lang_x64->Translate(handle,".sop0 = sop1 * sop2;",insn,true);
                    if (reg0 != NULL) {
                        PrintLine(insn,0,reg0);
                        num++;
                        free(reg0);
                    }
                    break;
            }
            break;
        case X86_INS_IDIV:
            reg0 = lang_x64->Translate(handle,".s_rdx = s_rax % sop0;:.s_rax = idiv(s_rax,sop0);",insn,true);
            //reg0 = lang_x64->Translate(handle,".s_rdx = s_rax % sop0;:.s_rax = s_rax / sop0;",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_CDQ:
            PrintLine(insn,1,"");
            num++;
            break;
        case X86_INS_LEA:
            /*
            reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
            if (IsRIP(insn->detail->x86.operands[1].mem.base)) {
                // lea		rdx, qword ptr [rip + 0x199f7]
                PrintLine(insn,1,lang_x64->E_MOV_RI,reg0,insn->address+insn->size+insn->detail->x86.operands[1].mem.disp);
            }
            else {
                // lea		rbp, qword ptr [rsp - 0x78]
                reg1 = lang_x64->reg_name(handle,insn->detail->x86.operands[1].mem.base);
                PrintLine(insn,1,lang_x64->E_LEA_M,reg0,reg1,insn->detail->x86.operands[1].mem.disp);
                free(reg1);
            }
            free(reg0);
            */
            reg0 = lang_x64->Translate(handle,".op0 = mem1;",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_MOVABS:
            // movabs		rdx, 0x5f3ae6d888f298a2
            reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
            PrintLine(insn,1,lang_x64->E_MOV_RI,reg0,insn->detail->x86.operands[1].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_SETNE:
            //reg0 = lang_x64->Translate(handle,".if get_zf()==false then op0 = 1 else op0 = 0 fi;",insn,true);
            reg0 = lang_x64->Translate(handle,".if !get_zf() then op0 = 1 else op0 = 0 fi;",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_SETE:
            //reg0 = lang_x64->Translate(handle,".if get_zf()==true then op0 = 1 else op0 = 0 fi;",insn,true);
            reg0 = lang_x64->Translate(handle,".if get_zf() then op0 = 1 else op0 = 0 fi;",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_XCHG:
            reg0 = lang_x64->Translate(handle,".push(bits,op0);:.op0 = op1;:.op1 = pop(bits);",insn,true);
            //reg0 = lang_x64->Translate(handle,".tmp0 = op0;:.op0 = op1;:.op1 = tmp0;",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_PSHUFD:
            reg0 = lang_x64->Translate(handle,".op0 = pshufd(op1,op2);",insn,true);
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            break;
        case X86_INS_MOVAPS:
        case X86_INS_MOVUPS:
        case X86_INS_MOVD:
        case X86_INS_MOVQ:
        case X86_INS_MOVZX:
        case X86_INS_MOV:
            if (IsFSGS(insn)) {
                reg0 = lang_x64->Translate(handle,".;",insn,true);
            }
            else {
                reg0 = lang_x64->Translate(handle,".op0 = op1;",insn,true);
            }
            if (reg0 != NULL) {
                PrintLine(insn,0,reg0);
                num++;
                free(reg0);
            }
            /*
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang_x64->reg_name(handle,insn->detail->x86.operands[0].reg);
                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                    reg1 = lang_x64->reg_name(handle,insn->detail->x86.operands[1].reg);
                    // mov		rsi, rcx
                    PrintLine(insn,1,lang_x64->E_MOV_RR,reg0,reg1);
                    free(reg1);
                    num++;
                }
                else if (insn->detail->x86.operands[1].type == X86_OP_IMM) {
                    // mov      dl, 0x01
                    PrintLine(insn,1,lang_x64->E_MOV_RI,reg0,insn->detail->x86.operands[1].imm);
                    num++;
                }
                else if (insn->detail->x86.operands[1].type == X86_OP_MEM) {
                    if (IsRIP(insn->detail->x86.operands[1].mem.base)) {
                        // mov rax, qword ptr [**rip** + 0x1dc97]
                        addr = insn->address + insn->size + insn->detail->x86.operands[1].mem.disp;
                        PrintLine(insn,1,lang_x64->E_MOV_RP,reg0,lang_x64->ptr(insn->detail->x86.operands[0]),addr);
                        num++;
                    }
                    else {
                        // mov	rbx, qword ptr [r14 + 8]
                        mstr = lang_x64->mem_str(handle,insn->detail->x86.operands[1]);
                        PrintLine(insn,1,lang_x64->E_MOV_RM,reg0,lang_x64->ptr(insn->detail->x86.operands[0]),mstr);
                        free(mstr);
                        num++;
                    }
                }
                free(reg0);
            }
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                    reg1 = lang_x64->reg_name(handle,insn->detail->x86.operands[1].reg);
                    if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                        // mov		qword ptr [rip + 0x1d8f1], rax
                        addr = insn->address + insn->size + insn->detail->x86.operands[0].mem.disp;
                        PrintLine(insn,1,lang_x64->E_MOV_PR,lang_x64->ptr(insn->detail->x86.operands[0]),addr,reg1);
                        num++;
                    }
                    else {
                        // mov		qword ptr [rsp + 0x1b0], rdi
                        mstr = lang_x64->mem_str(handle,insn->detail->x86.operands[0]);
                        PrintLine(insn,1,lang_x64->E_MOV_MR,lang_x64->ptr(insn->detail->x86.operands[0]),mstr,reg1);
                        free(mstr);
                        num++;
                    }
                    free(reg1);
                }
                else if (insn->detail->x86.operands[1].type == X86_OP_IMM) {
                    if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                        //  mov		dword ptr [rip + 0x1d725], 0xc0000409
                        uint64_t addr = insn->address + insn->size + insn->detail->x86.operands[0].mem.disp;
                        PrintLine(insn,1,lang_x64->E_MOV_PI,lang_x64->ptr(insn->detail->x86.operands[0]),addr,insn->detail->x86.operands[1].imm);
                        num++;
                    }
                    else {
                        // mov		dword ptr [rbp + 0x58], 0x6c6c642e
                        mstr = lang_x64->mem_str(handle,insn->detail->x86.operands[0]);
                        PrintLine(insn,1,lang_x64->E_MOV_MI,lang_x64->ptr(insn->detail->x86.operands[0]),mstr,insn->detail->x86.operands[1].imm);
                        free(mstr);
                        num++;
                    }
                }
            }
            */
            break;
        case X86_INS_CALL:
            //uint64_t addr = 0;
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                // call            0x180002240
                addr = insn->detail->x86.operands[0].imm;
                name = c->GetFunctionName(addr);
                if (name != NULL) {
                    PrintLine(insn,1,lang_x64->E_FUNC_NAME,name);
                    free(name);
                }
                else {
                    if (!Excluded(addr)) {
                        PrintLine(insn,1,lang_x64->E_FUNC_ADDR,addr);
                    }
                    else {
                        PrintLine(insn,0,lang_x64->E_SPACE);
                    }
                }
                num++;
            } 
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                    addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;
                    if (arch->IsImportFunction(addr,&lib,&func)) {
                        PrintLine(insn,1,lang_x64->E_CALL_FROM_IAT,lib,func);
                        free(lib);
                        free(func);
                        num++;
                    }    
                }
                else if ((insn->detail->x86.operands[0].mem.base == X86_OP_INVALID) && 
                        (insn->detail->x86.operands[0].mem.index == X86_OP_INVALID)) {
                    if (arch->Get_Address_At(insn->detail->x86.operands[0].mem.disp,&addr,insn->detail->x86.addr_size*8)) {
                        if (arch->ValidMemory(addr)) {
                            if (!Excluded(addr)) {
                                PrintLine(insn,1,lang_x64->E_FUNC_ADDR,addr);
                            }
                            else {
                                PrintLine(insn,0,lang_x64->E_SPACE);
                            }
                            num++;
                        }
                    }
                    /*
                    if (arch->ValidMemory(addr)) {
                        PrintLine(insn,1,lang_x64->E_FUNC_ADDR,addr);
                        num++;
                    }
                    */
                }
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
                sprintf(buffer,lang_x64->OP_REG,reg_name(handle,insn->detail->x86.operands[n].reg));
                strcat(params,buffer);
                break;
            case X86_OP_IMM:
                strcat(subname,"i");
                sprintf(buffer,lang_x64->OP_IMM,insn->detail->x86.operands[n].imm);
                strcat(params,buffer);
                break;
            case X86_OP_MEM:
                strcat(subname,"m");
                sprintf(buffer,lang_x64->OP_MEM,reg_name(handle,insn->detail->x86.operands[n].mem.base),
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
        PrintLine(insn,1,lang_x64->OP_SUBNAME,subname,insn->mnemonic,params);
    } else {
        PrintLine(insn,1,lang_x64->OP_ALONE,insn->mnemonic);
    }
    return (num+1);
}

void Base_x64::PrintSubCode(Code *c,int num) {
struct _subcode *sc;
int id;

    id = c->subcodes[num].id;
    char *name = c->GetFunctionName(c->subcodes[num].first);
    if (name != NULL) {
        //printf(C_FUNC_HEADER_NAME,name,c->subcodes[num].first);
        lang_x64->PrintFuncHeaderName(c,num,name);
        free(name);
    }
    else {
        //printf(C_FUNC_HEADER_ADDR,c->subcodes[num].first);
        lang_x64->PrintFuncHeaderAddr(c,num);
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
                PrintLabel(c,sc->insn[n].address);
                n = PrintInst(c,sc,n);
            }
        }
    }
    //printf(C_FUNC_FOOTER);
    lang_x64->PrintFuncFooter(c,num);
}

/*
void Base_x64::PrintSubMem(Code *c,int num) {
struct _submem *sm;
char sub[128];

    sm = &c->submems[num];
    char *buffer = (char *) malloc((sm->size*4)+128);
    buffer[0] = 0;
    for (int n=0;n<sm->size;n++) {
        sprintf(sub,"\\x%02x",sm->mem[n]);
        strcat(buffer,sub);
    }
    printf("    add_mem (&cpu,0x%llx,\"%s\",%i);\n",sm->addr,buffer,sm->size);
    free(buffer);
}
*/

void Base_x64::PrintCode(Code *c) {
struct _Section *sections;
int count,n;

    /*
    printf(C_HEADER);
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            if (c->subcodes[n].name != NULL) {
                printf(C_FUNC_NAME,c->subcodes[n].name,c->subcodes[n].first);
            }
            else {
                printf(C_FUNC_ADDR,c->subcodes[n].first);
            }
        }
    }
    */
    c->AddSubMem(STACK_ADDR,NULL,STACK_SIZE);
    lang_x64->PrintHeader(c);
    printf("\n");
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            PrintSubCode(c,n);
        }
    }
    //printf(C_FOOTER_1);
    lang_x64->PrintMainOpen(c);
    for (int n=0;n<c->submem_count;n++) {
        //PrintSubMem(c,n);
        lang_x64->PrintSubMem(c,n);
    }
    if (loadm) {
        sections = arch->GetSections(&count);
        if (sections != NULL) {
            for (n=0;n<count;n++) {
                PrintLine(NULL,1,lang_x64->E_LOAD_MEM,arch->GetFileName(),sections[n].d_Offset,sections[n].d_Size,sections[n].v_Address,sections[n].v_Size);
            }
            free(sections);
        }
    }
    printf(lang_x64->E_STACK_INIT,lang_x64->reg_name(handle,X86_REG_RSP),STACK_ADDR+(STACK_SIZE/2),
                                lang_x64->reg_name(handle,X86_REG_RBP),
                                lang_x64->reg_name(handle,X86_REG_RSP));
    //printf(C_FOOTER_2,c->ep);
    lang_x64->PrintMainClose(c);
}

Base_x64::Base_x64(Archive *a,Language *l) : Butcher(a,l) {
    lang_x64 = (Lang_x64 *)l;
}