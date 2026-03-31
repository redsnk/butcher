#include "lang_x64.hpp"

#define MAX_STR_OP     (1024*5)

const char *Lang_x64::ptr(cs_x86_op op) {
    switch (op.size) {
        case 1:
            return("byte");
        case 2:
            return("word");
        case 4:
            return("dword");
        case 8:
            return("qword");
        case 10:
            return("xword");
        case 16:
            return("dqword");
    }
    return ("ptr_error");
}

char *Lang_x64::mem_str(csh handle,cs_x86_op op) {
char tmp[256];
char *buffer;

    // mov		ecx, dword ptr [r8 + rax*4 + 0x27b8]
    buffer = (char *) malloc(256);
    if (op.mem.base != X86_REG_INVALID) {
        sprintf(buffer,"%s",reg_name(handle,op.mem.base));
    }
    else {
        buffer[0] = 0;
    }
    if (op.mem.index != X86_REG_INVALID) {
        sprintf(tmp,"+%s*%i",reg_name(handle,op.mem.index),op.mem.scale);
        strcat (buffer,tmp);
    }
    if (op.mem.disp) {
        sprintf(tmp,"%+lld",op.mem.disp);
        //sprintf(tmp,"+0x%llx",op.mem.disp);
        strcat (buffer,tmp);
    }
    else if (!op.mem.disp && (op.mem.base == X86_REG_INVALID) && (op.mem.index == X86_REG_INVALID)) {
        // TODO: pop		dword ptr fs:[0]
        sprintf(tmp,"0");
        strcat (buffer,tmp);
    }
    return (buffer);
}

char *Lang_x64::op_str(csh handle,cs_x86_op op,int bits,int sign, int lset) {
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
            if (lset) {
                strcpy(buffer,str);
            }
            else {
                sprintf(buffer,(sign)?E_S_GET_MEM():E_GET_MEM(),ptr(op),str);
            }
            free(str);
            return (strdup(buffer));
        case X86_OP_IMM:
            switch (bits) {
                case 8:
                    sprintf(buffer,"0x%"PRIx8,(uint8_t)op.imm);
                    break;
                case 16:
                    sprintf(buffer,"0x%"PRIx16,(uint16_t)op.imm);
                    break;
                case 32:
                    sprintf(buffer,"0x%"PRIx32,(uint32_t)op.imm);
                    break;
                case 64:
                case 128:
                    sprintf(buffer,"0x%"PRIx64,op.imm);
                    break;
            }
            return (strdup(buffer));
    }
    return(NULL);
}

char *Lang_x64::get_op_str(csh handle,cs_x86_op op,int bits,int sign) {
    return(op_str(handle,op,bits,sign,false));
}

char *Lang_x64::set_op_str(csh handle,cs_x86_op op,int bits,int sign) {
    return(op_str(handle,op,bits,sign,true));
}

/*
char *Lang_x64::Translate_reg (cs_insn *insn,const char *reg8,const char *reg16,const char *reg32,const char *reg64) {
    switch (insn->detail->x86.addr_size*8) {
        case 8:
            return (strdup(reg8));
        case 16:
            return (strdup(reg16));
        case 32:
            return (strdup(reg32));
        case 64:
            return (strdup(reg64));
    }
}
*/

char *Lang_x64::Translate_reg (csh handle,cs_insn *insn,int reg8,int reg16,int reg32,int reg64,int sign) {
int bits;

    if (insn->detail->x86.op_count == 0) {
        bits = insn->detail->x86.addr_size*8;
    }
    else {
        bits = insn->detail->x86.operands[0].size*8;
    }
    switch (bits) {
        case 8:
            return (sign?s_reg_name(handle,reg8):reg_name(handle,reg8));
        case 16:
            return (sign?s_reg_name(handle,reg16):reg_name(handle,reg16));
        case 32:
            return (sign?s_reg_name(handle,reg32):reg_name(handle,reg32));
        case 64:
            return (sign?s_reg_name(handle,reg64):reg_name(handle,reg64));
    }
}

char *Lang_x64::Translate_var (csh handle,cs_insn *insn,char *name,int lset) {
int bits;

    bits = insn->detail->x86.addr_size * 8;
    if (!strcmp(name,"op0")) {
        bits = insn->detail->x86.operands[0].size*8;
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[0],bits,false));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[0],bits,false));
        }
    }
    else if (!strcmp(name,"op1")) {
        bits = insn->detail->x86.operands[1].size*8;
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[1],bits,false));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[1],bits,false));
        }
    }
    else if (!strcmp(name,"op2")) {
        bits = insn->detail->x86.operands[2].size*8;
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[2],bits,false));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[2],bits,false));
        }
    }
    else if (!strcmp(name,"sop0")) {
        bits = insn->detail->x86.operands[0].size*8;
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[0],bits,true));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[0],bits,true));
        }
    }
    else if (!strcmp(name,"sop1")) {
        bits = insn->detail->x86.operands[1].size*8;
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[1],bits,true));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[1],bits,true));
        }
    }
    else if (!strcmp(name,"sop2")) {
        bits = insn->detail->x86.operands[2].size*8;
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[2],bits,true));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[2],bits,true));
        }
    }
    else if (!strcmp(name,"mem0")) {
        return(mem_str(handle,insn->detail->x86.operands[0]));
    }
    else if (!strcmp(name,"mem1")) {
        return(mem_str(handle,insn->detail->x86.operands[1]));
    }
    else if (!strcmp(name,"rax")) {
        //return(Translate_reg(insn,"_al","_ax","_eax","_rax"));
        return(Translate_reg(handle,insn,X86_REG_AL,X86_REG_AX,X86_REG_EAX,X86_REG_RAX,false));
    }
    else if (!strcmp(name,"rbx")) {
        //return(Translate_reg(insn,"_bl","_bx","_ebx","_rbx"));
        return(Translate_reg(handle,insn,X86_REG_BL,X86_REG_BX,X86_REG_EBX,X86_REG_RBX,false));
    }
    else if (!strcmp(name,"rcx")) {
        //return(Translate_reg(insn,"_cl","_cx","_ecx","_rcx"));
        return(Translate_reg(handle,insn,X86_REG_CL,X86_REG_CX,X86_REG_ECX,X86_REG_RCX,false));
    }
    else if (!strcmp(name,"rdx")) {
        //return(Translate_reg(insn,"_dl","_dx","_edx","_rdx"));
        return(Translate_reg(handle,insn,X86_REG_DL,X86_REG_DX,X86_REG_EDX,X86_REG_RDX,false));
    }
    else if (!strcmp(name,"s_rax")) {
        //return(Translate_reg(insn,"s_al","s_ax","s_eax","s_rax"));
        return(Translate_reg(handle,insn,X86_REG_AL,X86_REG_AX,X86_REG_EAX,X86_REG_RAX,true));
    }
    else if (!strcmp(name,"s_rbx")) {
        //return(Translate_reg(insn,"s_bl","s_bx","s_ebx","s_rbx"));
        return(Translate_reg(handle,insn,X86_REG_BL,X86_REG_BX,X86_REG_EBX,X86_REG_RBX,true));
    }
    else if (!strcmp(name,"s_rcx")) {
        //return(Translate_reg(insn,"s_cl","s_cx","s_ecx","s_rcx"));
        return(Translate_reg(handle,insn,X86_REG_CL,X86_REG_CX,X86_REG_ECX,X86_REG_RCX,true));
    }
    else if (!strcmp(name,"s_rdx")) {
        //return(Translate_reg(insn,"s_dl","s_dx","s_edx","s_rdx"));
        return(Translate_reg(handle,insn,X86_REG_DL,X86_REG_DX,X86_REG_EDX,X86_REG_RDX,true));
    }
    else if (!strcmp(name,"rsp")) {
        //return(Translate_reg(insn,"<sp 8bits>","_sp","_esp","_rsp"));
        return(Translate_reg(handle,insn,X86_REG_INVALID,X86_REG_SP,X86_REG_ESP,X86_REG_RSP,false));
    }
    else if (!strcmp(name,"rbp")) {
        //return(Translate_reg(insn,"<sp 8bits>","_sp","_esp","_rsp"));
        return(Translate_reg(handle,insn,X86_REG_INVALID,X86_REG_BP,X86_REG_EBP,X86_REG_RBP,false));
    }
    else if (!strcmp(name,"zf")) {
        return (strdup(F_SET_ZF()));
    }
    else if (!strcmp(name,"sf")) {
        return (strdup(F_SET_SF()));
    }
    else if (!strcmp(name,"cf")) {
        return (strdup(F_SET_CF()));
    }
    else if (!strcmp(name,"of")) {
        return (strdup(F_SET_OF()));
    }
    else if (!strcmp(name,"get_zf")) {
        return (strdup(F_GET_ZF()));
    }
    else if (!strcmp(name,"get_sf")) {
        return (strdup(F_GET_SF()));
    }
    else if (!strcmp(name,"get_cf")) {
        return (strdup(F_GET_CF()));
    }
    else if (!strcmp(name,"get_of")) {
        return (strdup(F_GET_OF()));
    }
    else if (!strcmp(name,"add_of")) {
        return (strdup(F_ADD_OF()));
    }
    else if (!strcmp(name,"add_cf")) {
        return (strdup(F_ADD_CF()));
    }
    /*
    else if (!strcmp(name,"num_cf")) {
        return (strdup(F_NUM_CF()));
    }
    */
    else if (!strcmp(name,"sub_of")) {
        return (strdup(F_SUB_OF()));
    }
    else if (!strcmp(name,"sdiv")) {
        return (strdup(F_IDIV()));
    }
    else if (!strcmp(name,"bits")) {
        char *buffer = (char *) malloc(32);
        //sprintf(buffer,"%i",insn->detail->x86.addr_size*8);
        sprintf(buffer,"%i",bits);
        return (buffer);
    }
    else if (!strcmp(name,"true")) {
        return (strdup(F_TRUE()));
    }
    else if (!strcmp(name,"false")) {
        return (strdup(F_FALSE()));
    }
    else if (!strcmp(name,"push")) {
        return (strdup(F_PUSH()));
    }
    else if (!strcmp(name,"pop")) {
        return (strdup(F_POP()));
    }
    else if (!strcmp(name,"pow")) {
        return (strdup(F_POW()));
    }
    else if (!strcmp(name,"not")) {
        return (strdup(F_NOT()));
    }
    else if (!strcmp(name,"neg")) {
        return (strdup(F_NEG()));
    }
    else if (!strcmp(name,"pshufd")) {
        return (strdup(F_PSHUFD()));
    }
    else if (!strcmp(name,"anoncall")) {
        return (strdup(E_ANONC()));
    }
    else if (!strcmp(name,"anonjmp")) {
        //return (strdup("anon"));
        return (strdup(E_ANONJ()));
    }
    else if (!strcmp(name,"anonlabel")) {
        //return (strdup("label_Anon"));
        return (strdup(E_LABEL_ANONJ()));
    }
    else if (!strcmp(name,"pushfpu")) {
        return (strdup(F_PUSHFPU()));
    }
    else if (!strcmp(name,"popfpu")) {
        return (strdup(F_POPFPU()));
    }
    else if (!strcmp(name,"st0")) {
        return (reg_name(handle,X86_REG_ST0));
    }
    else if (!strcmp(name,"st1")) {
        return (reg_name(handle,X86_REG_ST1));
    }
    else if (!strcmp(name,"s_st0")) {
        return (s_reg_name(handle,X86_REG_ST0));
    }
    else if (!strcmp(name,"s_st1")) {
        return (s_reg_name(handle,X86_REG_ST1));
    }
    else if (!strcmp(name,"break")) {
        return (strdup(E_BREAK()));
    }
    return (strdup("<Translate_var error>"));
}

char *Lang_x64::Translate_item (csh handle,cs_insn *insn,_s_item *i,int lset) {
char *buffer;

    switch (i->id) {
        case _id_item::NAME:
            return (Translate_var(handle,insn,i->item.name,lset));
        case _id_item::NUMBER:
            buffer = (char *) malloc(MAX_STR_OP);
            sprintf(buffer,"%lld",i->item.num);
            return (buffer);
        case _id_item::RESULT:
            return (strdup(i->item.name));
        default:
            break;
    }
    return (strdup("<Translate_item  error>"));
}

char *Lang_x64::Translate (csh handle,char *s, cs_insn *insn,int ends) {
Emit *e;
char *buffer,*it1,*it2,*it3;
const char *op;
int lmem;
char tmp[MAX_STR_OP];
int n;

    buffer = (char *) malloc(MAX_STR_OP);
    buffer[0] = 0;
    e = Parse(s);
    n = 0;
    while (n < e->count) {
        op = NULL;
        switch (e->items[n].id) {
            case _id_item::INDENT:
                /*
                strcat (buffer,INDENT());
                e->del_item(n);
                n -= 1;
                */
                e->res_item(n,(char *)INDENT());
                break;
            case _id_item::LF:
                /*
                strcat (buffer,"\n");
                e->del_item(n);
                n -= 1;
                */
                sprintf(tmp,"%s\n",(char *)ENDS());
                e->res_item(n,tmp);
                break;
            case _id_item::ENC:
                it1 = Translate_item(handle,insn,&e->items[n-1],false);
                sprintf(tmp,"(%s)",it1);
                free(it1);
                e->res_item(n,tmp);
                e->del_item(n-1);
                n -= 1;
                break;
            case _id_item::ADD:
                if (op == NULL) op = "+";
            case _id_item::SUB:
                if (op == NULL) op = "-";
            case _id_item::MUL:
                if (op == NULL) op = "*";
            case _id_item::DIV:
                if (op == NULL) op = "/";
            case _id_item::MOD:
                if (op == NULL) op = "%%";
            case _id_item::SHL:
                if (op == NULL) op = SHL();
            case _id_item::SHR:
                if (op == NULL) op = SHR();
            case _id_item::EQUAL:
                if (op == NULL) op = EQUAL();
            case _id_item::NEQUAL:
                if (op == NULL) op = NEQUAL();
            case _id_item::GT:
                if (op == NULL) op = GT();
            case _id_item::LT:
                if (op == NULL) op = LT();
            case _id_item::GTE:
                if (op == NULL) op = GTE();
            case _id_item::LTE:
                if (op == NULL) op = LTE();
            case _id_item::AND:
                if (op == NULL) op = AND();
            case _id_item::OR:
                if (op == NULL) op = OR();
            case _id_item::XOR:
                if (op == NULL) op = XOR();
            case _id_item::LIST:
                if (op == NULL) op = ",";
            case _id_item::JOIN:
                if (op == NULL) op = ";";
                it1 = Translate_item(handle,insn,&e->items[n-2],false);
                it2 = Translate_item(handle,insn,&e->items[n-1],false);
                sprintf(tmp,"%s%s%s",it1,op,it2);
                free(it2);
                free(it1);
                e->res_item(n,tmp);
                e->del_item(n-2);
                e->del_item(n-2);
                n -= 2;
                break;
            case _id_item::ASSIGN:
                it1 = Translate_var(handle,insn,e->items[n].item.name,true);
                lmem = false;
                if (!strcmp(e->items[n].item.name,"op0") || !strcmp(e->items[n].item.name,"sop0")) {
                    lmem = (insn->detail->x86.operands[0].type ==X86_OP_MEM);
                }
                else if (!strcmp(e->items[n].item.name,"op1") || !strcmp(e->items[n].item.name,"sop1")) {
                    lmem = (insn->detail->x86.operands[1].type ==X86_OP_MEM);
                }
                it2 = Translate_item(handle,insn,&e->items[n-1],false);
                if (lmem) {
                    sprintf(tmp,E_SET_MEM(),ptr(insn->detail->x86.operands[0]),it1,it2);
                }
                else {
                    sprintf(tmp,"%s = %s",it1,it2);
                }
                free(it2);
                free(it1);
                e->res_item(n,tmp);
                e->del_item(n-1);
                n -= 1;
                break;
            case _id_item::FUNC_VOID:
                it1 = Translate_var(handle,insn,e->items[n].item.name,false);
                sprintf(tmp,"%s%s",it1,ENDF());
                free(it1);
                e->res_item(n,tmp);
                break;
            case _id_item::FUNCTION:
                it1 = Translate_var(handle,insn,e->items[n].item.name,false);
                it2 = Translate_item(handle,insn,&e->items[n-1],false);
                sprintf(tmp,"%s%s%s",it1,it2,ENDF());
                free(it2);
                free(it1);
                e->res_item(n,tmp);
                e->del_item(n-1);
                n -= 1;
                break;
            case _id_item::IFTHEN:
                it1 = Translate_item(handle,insn,&e->items[n-2],false);
                it2 = Translate_item(handle,insn,&e->items[n-1],false);
                sprintf(tmp,E_IFTHEN(),it1,it2);
                free(it2);
                free(it1);
                e->res_item(n,tmp);
                e->del_item(n-2);
                e->del_item(n-2);
                n -= 2;
                break;
            case _id_item::IFTHENELSE:
                it1 = Translate_item(handle,insn,&e->items[n-3],false);
                it2 = Translate_item(handle,insn,&e->items[n-2],false);
                it3 = Translate_item(handle,insn,&e->items[n-1],false);
                sprintf(tmp,E_IFTHENELSE(),it1,it2,it3);
                free(it3);
                free(it2);
                free(it1);
                e->res_item(n,tmp);
                
                e->del_item(n-3);
                e->del_item(n-3);
                e->del_item(n-3);
                n -= 3;
                
                /*
                e->del_item(n-3);
                e->del_item(n-3);
                n -= 2;
                */       
                break;
            case _id_item::WHILE:
                it1 = Translate_item(handle,insn,&e->items[n-2],false);
                it2 = Translate_item(handle,insn,&e->items[n-1],false);
                sprintf(tmp,E_WHILE(),it1,it2);
                free(it2);
                free(it1);
                e->res_item(n,tmp);
                e->del_item(n-2);
                e->del_item(n-2);
                n -= 2;
                break;
            case _id_item::NOT:
                it1 = Translate_item(handle,insn,&e->items[n-1],false);
                sprintf(tmp,"%s %s",E_NOT(),it1);
                free(it1);
                e->res_item(n,tmp);
                e->del_item(n-1);
                n -= 1;
                break;
            case _id_item::GOTOEXPR:
                it1 = Translate_item(handle,insn,&e->items[n-1],false);
                sprintf(tmp,"goto %s",it1);
                free(it1);
                e->res_item(n,tmp);
                e->del_item(n-1);
                n -= 1;
                break;
            case _id_item::BREAK:
                e->res_item(n,(char *)E_BREAK());
                break;
            /*
            case _id_item::END:
                if ((n > 0) && (e->items[n-1].id == RESULT)) {
                    it1 = Translate_item(handle,insn,&e->items[n-1],false);
                    strcat (buffer,it1);
                    free(it1);
                    e->del_item(n-1);
                    e->del_item(n-1);
                    n -= 2;
                }
                else {
                    e->del_item(n);
                    n -= 1;
                }
                if (ends) strcat(buffer,ENDS());
                break;
            */
            default:
                break;
        }
        n++;
    }
    // print stack
    for (n=0;n<e->count;n++) {
        if (e->items[n].id == RESULT) {
            it1 = Translate_item(handle,insn,&e->items[n],false);
            strcat (buffer,it1);
            free(it1);
        }
    }
    if (ends) strcat(buffer,ENDS());
    free(e);
    return (buffer);
}
