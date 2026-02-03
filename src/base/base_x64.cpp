#include "base_x64.hpp"
#include "../files/pe.hpp"
#include <capstone/capstone.h>

const char *reg_name(csh handle,int id_reg) {
    if (id_reg == X86_REG_INVALID) {
        return ("");
    }
    return (cs_reg_name(handle,id_reg));
}

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

const char *ptr(cs_insn *insn) {
    switch (insn->detail->x86.operands[0].size) {
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

int ClearFlagInst(cs_insn *insn) {
    switch(insn->id) {
        case X86_INS_SUB:
        case X86_INS_ADD:
        case X86_INS_XOR:
        case X86_INS_ADC:
        case X86_INS_SBB:
        case X86_INS_CMP:
        case X86_INS_TEST:
            return (true);
    }
    return (false);
}

int UseFlagInst(cs_insn *insn) {
    if (JccInst(insn)) {
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
    return (false);
}

/*
int Base_x64::IsJmp(cs_insn *insn, uint64_t *addr) {
    *addr = 0;
    if (insn->id == X86_INS_JMP) {
        // TODO: [rip + XXXX]
        if (insn->detail->x86.op_count == 1) {
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                *addr = insn->detail->x86.operands[0].imm;
            }
        }
        return (true);
    }
    return (false);
}
*/

#define MAX_JMPS      (1024*10)

int Base_x64::IsJmp(cs_insn *insn, uint64_t *addr[],int *count) {
int n,b,c;
uint8_t *mem;
uint64_t read,a;

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
                    mem = arch->GetMemory(a,b,&read);
                    if ((mem != NULL) && (read == b)) {
                        if (b == 4) {
                            a = memto32(mem);
                        }
                        else if (b == 8) {
                            a = memto64(mem);
                        }
                        free(mem);
                        if (arch->ValidMemory(a)) {
                            // Valid
                            if (c == 0) {
                                *addr = (uint64_t *) malloc(sizeof(uint64_t));
                            }
                            else {
                                *addr = (uint64_t *) realloc(*addr,sizeof(uint64_t)*(c+1));
                            }
                            (*addr)[c++] = a;
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
        strcat (out,lang->INDENT);
    }
    strcat (out,buffer);
    char *p = out;
    /*
    for (int n=0;n<strlen(buffer);n++) {
        if (buffer[n] == '\n') {
            p = buffer+n+1;
        }
    }
    */
    //while (strlen(buffer) < lang->COMM_SEP) strcat(buffer," ");
    while (strlen(p) < lang->COMM_SEP) strcat(p," ");
    printf("%s%s 0x%llx:\t%s\t\t%s\n", out, lang->COMM, insn->address, insn->mnemonic,insn->op_str);
}

int Base_x64::PrintExtra(Code *c,struct _subcode *sc,int num) {
cs_insn *insn;
uint64_t read;
char *reg0,*reg1,*mstr;
uint64_t addr;
char *lib,*func,*name;
uint8_t *mem;
int n,b;

    insn = &sc->insn[num];
    switch (insn->id) {
        case X86_INS_RET:
            // ret
            PrintLine(insn,1,lang->E_RETURN);
            num++;
            break;
        case X86_INS_JMP:
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                PrintLine(insn,1,lang->E_GOTO,insn->detail->x86.operands[0].imm);
                num++;
            }
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                    addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;
                    if (arch->IsImportFunction(addr,&lib,&func)) {
                        PrintLine(insn,1,lang->E_JMP_FROM_IAT,lib,func);
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
                        PrintLine(insn,1,lang->E_JMP_FROM_IAT,lib,func);
                        free(lib);
                        free(func);
                        num++;
                    }
                }
                else if((insn->detail->x86.operands[0].mem.base == X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.index != X86_REG_INVALID) && 
                        (insn->detail->x86.operands[0].mem.scale > 1) && (insn->detail->x86.operands[0].mem.disp)) {
                    // jmp             dword ptr [eax*4 + 0x44fcb89]
                    PrintLine(insn,1,"switch(%s) {",lang->reg_name(handle,insn->detail->x86.operands[0].mem.index));
                    for (n=0;n<MAX_JMPS;n++) {
                        addr = insn->detail->x86.operands[0].mem.disp + (insn->detail->x86.operands[0].mem.scale * n);
                        b = insn->detail->x86.addr_size;
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
                                PrintLine(insn,2,"case %i:",n);
                                PrintLine(insn,3,lang->E_GOTO,addr);
                            }
                            else {
                                break;
                            }
                        }
                        else {
                            break;
                        }
                    }
                    PrintLine(insn,1,"}",lang->reg_name(handle,insn->detail->x86.operands[0].mem.base));
                    num++;
                }
            }
            break;
        case X86_INS_JE:
            PrintLine(insn,1,lang->E_JE,insn->detail->x86.operands[0].imm);
            num++;
            break;
        case X86_INS_JNE:
            PrintLine(insn,1,lang->E_JNE,insn->detail->x86.operands[0].imm);
            num++;
            break;
        case X86_INS_JA:
            PrintLine(insn,1,lang->E_JA,insn->detail->x86.operands[0].imm);
            num++;
            break;
        case X86_INS_JAE:
            PrintLine(insn,1,lang->E_JAE,insn->detail->x86.operands[0].imm);
            num++;
            break;
        case X86_INS_PUSH:
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
                PrintLine(insn,1,lang->E_PUSH,ptr(insn),reg0);
                free(reg0);
                num++;
            }
            break;
        case X86_INS_POP:
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
                PrintLine(insn,1,lang->E_POP,reg0,ptr(insn));
                free(reg0);
                num++;
            }
            break;
        case X86_INS_SUB:
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                    reg1 = lang->reg_name(handle,insn->detail->x86.operands[1].reg);
                    //sub		rsp, rax
                    if (FlagsNotUsed(sc,num)) {
                        PrintLine(insn,1,lang->E_SUB_RR,reg0,reg0,reg1);
                        num++;
                    }
                    free(reg1);
                }
                else if (insn->detail->x86.operands[1].type == X86_OP_IMM) {
                    //sub		rsp, 0x178
                    if (FlagsNotUsed(sc,num)) {
                        PrintLine(insn,1,lang->E_SUB_RI,reg0,reg0,insn->detail->x86.operands[1].imm);
                        num++;
                    }
                }
                free(reg0);
            }
            break;
        case X86_INS_ADD:
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                    reg1 = lang->reg_name(handle,insn->detail->x86.operands[1].reg);
                    //add		rsp, rax
                    if (FlagsNotUsed(sc,num)) {
                        PrintLine(insn,1,lang->E_ADD_RR,reg0,reg0,reg1);
                        num++;
                    }
                    free(reg1);
                }
                else if (insn->detail->x86.operands[1].type == X86_OP_IMM) {
                    //add		rsp, 0x178
                    if (FlagsNotUsed(sc,num)) {
                        PrintLine(insn,1,lang->E_ADD_RI,reg0,reg0,insn->detail->x86.operands[1].imm);
                        num++;
                    }
                }
                free(reg0);
            }
            break;
        case X86_INS_XOR:
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                    reg1 = lang->reg_name(handle,insn->detail->x86.operands[1].reg);
                    //xor		rsp, rax
                    if (FlagsNotUsed(sc,num)) {
                        if (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg) {
                            PrintLine(insn,1,lang->E_XOR_R,reg0);
                        }
                        else {
                            PrintLine(insn,1,lang->E_XOR_RR,reg0,reg0,reg1);
                        }
                        num++;
                    }
                    free(reg1);
                }
                else if (insn->detail->x86.operands[1].type == X86_OP_IMM) {
                    //xor		rsp, 0x178
                    if (FlagsNotUsed(sc,num)) {
                        PrintLine(insn,1,lang->E_XOR_RI,reg0,reg0,insn->detail->x86.operands[1].imm);
                        num++;
                    }
                }
                free(reg0);
            }
            break;
        case X86_INS_TEST:
            if ((num+1) < sc->count) {
                cs_insn *next = &sc->insn[num+1];
                switch (next->id) {
                    case X86_INS_JNE:
                        if (FlagsNotUsed(sc,num+1)) {
                            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                                reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
                                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                                    if (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg) {
                                        // test		eax, eax
                                        // jne		0x1800026ce
                                        PrintLine(insn,0,lang->E_SPACE);
                                        PrintLine(next,1,lang->E_JNE_GOTO,reg0,next->detail->x86.operands[0].imm);
                                        num += 2;

                                    }
                                }
                                free(reg0);
                            }
                        }
                        break;
                    case X86_INS_JE:
                        if (FlagsNotUsed(sc,num+1)) {
                            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                                reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
                                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                                    if (insn->detail->x86.operands[0].reg == insn->detail->x86.operands[1].reg) {
                                        // test		eax, eax
                                        // je		0x1800026ce
                                        PrintLine(insn,0,lang->E_SPACE);
                                        PrintLine(next,1,lang->E_JE_GOTO,reg0,next->detail->x86.operands[0].imm);
                                        num += 2;

                                    }
                                }
                                free(reg0);
                            }
                        }
                        break;
                }
            }
            break;
        case X86_INS_LEA:
            reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
            if (IsRIP(insn->detail->x86.operands[1].mem.base)) {
                // lea		rdx, qword ptr [rip + 0x199f7]
                PrintLine(insn,1,lang->E_MOV_RI,reg0,insn->address+insn->size+insn->detail->x86.operands[1].mem.disp);
            }
            else {
                // lea		rbp, qword ptr [rsp - 0x78]
                reg1 = lang->reg_name(handle,insn->detail->x86.operands[1].mem.base);
                PrintLine(insn,1,lang->E_LEA_M,reg0,reg1,insn->detail->x86.operands[1].mem.disp);
                free(reg1);
            }
            free(reg0);
            num++;
            break;
        case X86_INS_MOVABS:
            // movabs		rdx, 0x5f3ae6d888f298a2
            reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
            PrintLine(insn,1,lang->E_MOV_RI,reg0,insn->detail->x86.operands[1].imm);
            free(reg0);
            num++;
            break;
        case X86_INS_MOV:
            if (insn->detail->x86.operands[0].type == X86_OP_REG) {
                reg0 = lang->reg_name(handle,insn->detail->x86.operands[0].reg);
                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                    reg1 = lang->reg_name(handle,insn->detail->x86.operands[1].reg);
                    // mov		rsi, rcx
                    PrintLine(insn,1,lang->E_MOV_RR,reg0,reg1);
                    free(reg1);
                    num++;
                }
                else if (insn->detail->x86.operands[1].type == X86_OP_IMM) {
                    // mov      dl, 0x01
                    PrintLine(insn,1,lang->E_MOV_RI,reg0,insn->detail->x86.operands[1].imm);
                    num++;
                }
                else if (insn->detail->x86.operands[1].type == X86_OP_MEM) {
                    if (IsRIP(insn->detail->x86.operands[1].mem.base)) {
                        // mov rax, qword ptr [**rip** + 0x1dc97]
                        addr = insn->address + insn->size + insn->detail->x86.operands[1].mem.disp;
                        n = insn->detail->x86.addr_size;
                        mem = arch->GetMemory(addr,n,&read);
                        if ((mem != NULL) && (read == n)) {
                            c->AddSubMem(addr,mem,n);
                            free(mem);
                        }
                        PrintLine(insn,1,lang->E_MOV_RP,reg0,ptr(insn),addr);
                        num++;
                    }
                    else {
                        // mov	rbx, qword ptr [r14 + 8]
                        mstr = lang->mem_str(handle,insn->detail->x86.operands[1]);
                        PrintLine(insn,1,lang->E_MOV_RM,reg0,ptr(insn),mstr);
                        free(mstr);
                        num++;
                    }
                }
                free(reg0);
            }
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if (insn->detail->x86.operands[1].type == X86_OP_REG) {
                    reg1 = lang->reg_name(handle,insn->detail->x86.operands[1].reg);
                    if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                        // mov		qword ptr [rip + 0x1d8f1], rax
                        addr = insn->address + insn->size + insn->detail->x86.operands[0].mem.disp;
                        PrintLine(insn,1,lang->E_MOV_PR,ptr(insn),addr,reg1);
                        num++;
                    }
                    else {
                        // mov		qword ptr [rsp + 0x1b0], rdi
                        mstr = lang->mem_str(handle,insn->detail->x86.operands[0]);
                        PrintLine(insn,1,lang->E_MOV_MR,ptr(insn),mstr,reg1);
                        free(mstr);
                        num++;
                    }
                    free(reg1);
                }
                else if (insn->detail->x86.operands[1].type == X86_OP_IMM) {
                    if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                        //  mov		dword ptr [rip + 0x1d725], 0xc0000409
                        uint64_t addr = insn->address + insn->size + insn->detail->x86.operands[0].mem.disp;
                        PrintLine(insn,1,lang->E_MOV_PI,ptr(insn),addr,insn->detail->x86.operands[1].imm);
                        num++;
                    }
                    else {
                        // mov		dword ptr [rbp + 0x58], 0x6c6c642e
                        mstr = lang->mem_str(handle,insn->detail->x86.operands[0]);
                        PrintLine(insn,1,lang->E_MOV_MI,ptr(insn),mstr,insn->detail->x86.operands[1].imm);
                        free(mstr);
                        num++;
                    }
                }
            }
            break;
        case X86_INS_CALL:
            //uint64_t addr = 0;
            if (insn->detail->x86.operands[0].type == X86_OP_IMM) {
                // call            0x180002240
                addr = insn->detail->x86.operands[0].imm;
                name = c->GetFunctionName(addr);
                if (name != NULL) {
                    PrintLine(insn,1,lang->E_FUNC_NAME,name);
                    free(name);
                }
                else {
                    PrintLine(insn,1,lang->E_FUNC_ADDR,addr);
                }
                num++;
            } 
            else if (insn->detail->x86.operands[0].type == X86_OP_MEM) {
                if (IsRIP(insn->detail->x86.operands[0].mem.base)) {
                    addr = insn->address+insn->size+insn->detail->x86.operands[0].mem.disp;
                    if (arch->IsImportFunction(addr,&lib,&func)) {
                        PrintLine(insn,1,lang->E_CALL_FROM_IAT,lib,func);
                        free(lib);
                        free(func);
                        num++;
                    }    
                }
            }
            break;
        default:
            // Reset history
            break;
    }
    return (num);
}

int Base_x64::PrintInst(Code *c,struct _subcode *sc,int num) {
int n;
char subname[1024];
char params[1024];
char buffer[1024];
cs_insn *insn;

    int ret = PrintExtra(c,sc,num);
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
                sprintf(buffer,lang->OP_REG,reg_name(handle,insn->detail->x86.operands[n].reg));
                strcat(params,buffer);
                break;
            case X86_OP_IMM:
                strcat(subname,"i");
                sprintf(buffer,lang->OP_IMM,insn->detail->x86.operands[n].imm);
                strcat(params,buffer);
                break;
            case X86_OP_MEM:
                strcat(subname,"m");
                sprintf(buffer,lang->OP_MEM,reg_name(handle,insn->detail->x86.operands[n].mem.base),
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
        PrintLine(insn,1,lang->OP_SUBNAME,subname,insn->mnemonic,params);
    } else {
        PrintLine(insn,1,lang->OP_ALONE,insn->mnemonic);
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
        lang->PrintFuncHeaderName(c,num,name);
        free(name);
    }
    else {
        //printf(C_FUNC_HEADER_ADDR,c->subcodes[num].first);
        lang->PrintFuncHeaderAddr(c,num);
    }
    for (int m=0;m<c->subcod_count;m++) {
        sc = &c->subcodes[m];
        if ((sc->id == id) || (sc->parent == id)) {
            if (sc->parent != SUBCODE_TOP) {
                lang->PrintSubCodeSep();
            }
            for (int n=0;n<sc->count;) {
                if (sc->insn[n].address > sc->last) {
                    break;
                }
                if (std::find(c->labels.begin(), c->labels.end(), sc->insn[n].address) != c->labels.end()) {
                    printf(lang->E_LABEL,sc->insn[n].address);
                }
                n = PrintInst(c,sc,n);
            }
        }
    }
    //printf(C_FUNC_FOOTER);
    lang->PrintFuncFooter(c,num);
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
    lang->PrintHeader(c);
    printf("\n");
    for (int n=0;n<c->subcod_count;n++) {
        if (c->subcodes[n].parent == SUBCODE_TOP) {
            PrintSubCode(c,n);
        }
    }
    //printf(C_FOOTER_1);
    lang->PrintMainOpen(c);
    for (int n=0;n<c->submem_count;n++) {
        //PrintSubMem(c,n);
        lang->PrintSubMem(c,n);
    }
    printf(lang->E_STACK_INIT,lang->reg_name(handle,X86_REG_RSP),STACK_ADDR+(STACK_SIZE/2),
                                lang->reg_name(handle,X86_REG_RBP),
                                lang->reg_name(handle,X86_REG_RSP));
    //printf(C_FOOTER_2,c->ep);
    lang->PrintMainClose(c);
}
