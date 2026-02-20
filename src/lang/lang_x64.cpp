#include "lang_x64.hpp"

#define MAX_STR_OP     (1024)

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
    }
    return ("ptr_error");
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
                sprintf(buffer,(sign)?E_S_GET_MEM:E_GET_MEM,ptr(op),str);
            }
            free(str);
            return (strdup(buffer));
        case X86_OP_IMM:
            switch (bits) {
                case 8:
                    sprintf(buffer,"0x%"PRIx8,(int8_t)op.imm);
                    break;
                case 16:
                    sprintf(buffer,"0x%"PRIx16,(int16_t)op.imm);
                    break;
                case 32:
                    sprintf(buffer,"0x%"PRIx32,(int32_t)op.imm);
                    break;
                case 64:
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

char *Lang_x64::Translate_var (csh handle,cs_insn *insn,char *name,int lset) {
int bits;

    bits = insn->detail->x86.addr_size * 8;
    if (!strcmp(name,"op0")) {
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[0],bits,false));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[0],bits,false));
        }
    }
    else if (!strcmp(name,"op1")) {
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[1],bits,false));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[1],bits,false));
        }
    }
    else if (!strcmp(name,"sop0")) {
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[0],bits,true));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[0],bits,true));
        }
    }
    else if (!strcmp(name,"sop1")) {
        if (lset) {
            return (set_op_str(handle,insn->detail->x86.operands[1],bits,true));
        }
        else {
            return (get_op_str(handle,insn->detail->x86.operands[1],bits,true));
        }
    }
    else if (!strcmp(name,"zf")) {
        return (strdup(F_ZF));
    }
    else if (!strcmp(name,"sf")) {
        return (strdup(F_SF));
    }
    else if (!strcmp(name,"cf")) {
        return (strdup(F_CF));
    }
    else if (!strcmp(name,"of")) {
        return (strdup(F_OF));
    }
    else if (!strcmp(name,"add_of")) {
        return (strdup(F_ADD_OF));
    }
    else if (!strcmp(name,"add_cf")) {
        return (strdup(F_ADD_CF));
    }
    else if (!strcmp(name,"sub_of")) {
        return (strdup(F_SUB_OF));
    }
    /*
    else if (!strcmp(name,"sub_cf")) {
        return (strdup(F_SUB_CF));
    }
    */
    else if (!strcmp(name,"bits")) {
        char *buffer = (char *) malloc(1024);
        sprintf(buffer,"%i",insn->detail->x86.addr_size*8);
        return (buffer);
    }
    else if (!strcmp(name,"true")) {
        return (strdup(F_TRUE));
    }
    else if (!strcmp(name,"false")) {
        return (strdup(F_FALSE));
    }
    else if (!strcmp(name,"push")) {
        return (strdup(F_PUSH));
    }
    else if (!strcmp(name,"pop")) {
        return (strdup(F_POP));
    }
    else if (!strcmp(name,"pow")) {
        return (strdup(F_POW));
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
char *buffer,*it1,*it2;
const char *op;
int lmem;
char tmp[MAX_STR_OP];
int n;
//std::vector<std::string> list;
//std::string str;

    buffer = (char *) malloc(MAX_STR_OP);
    buffer[0] = 0;
    e = Parse(s);
    n = 0;
    while (n < e->count) {
        op = NULL;
        switch (e->items[n].id) {
            case _id_item::INDENT:
                strcat (buffer,INDENT);
                e->del_item(n);
                n -= 1;
                break;
            case _id_item::LF:
                strcat (buffer,"\n");
                e->del_item(n);
                n -= 1;
                break;
            case _id_item::ENC:
                /*
                if (list.size()>0) {
                    str = list.back();
                    list.pop_back();
                    sprintf(tmp,"(%s)",str.c_str());
                    list.push_back(tmp);
                    e->del_item(n);
                    n -= 1;
                }
                else {
                    it1 = Translate_item(handle,insn,&e->items[n-1],false);
                    sprintf(tmp,"(%s)",it1);
                    list.push_back(tmp);
                    free(it1);
                    e->del_item(n-1);
                    e->del_item(n-1);
                    n -= 2;
                }
                */
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
            case _id_item::EQUAL:
                if (op == NULL) op = EQUAL;
            case _id_item::NEQUAL:
                if (op == NULL) op = NEQUAL;
            case _id_item::GT:
                if (op == NULL) op = GT;
            case _id_item::LT:
                if (op == NULL) op = LT;
            case _id_item::GTE:
                if (op == NULL) op = GTE;
            case _id_item::LTE:
                if (op == NULL) op = LTE;
            case _id_item::AND:
                if (op == NULL) op = AND;
            case _id_item::OR:
                if (op == NULL) op = OR;
            case _id_item::XOR:
                if (op == NULL) op = XOR;
            case _id_item::LIST:
                if (op == NULL) op = ",";
                /*
                if (list.size()>0) {
                    it1 = Translate_item(handle,insn,&e->items[n-1],false);
                    str = list.back();
                    list.pop_back();
                    sprintf(tmp,"%s%s%s",it1,op,str.c_str());
                    //sprintf(tmp,"%s%s%s",str.c_str(),op,it1);
                    list.push_back(tmp);
                    free(it1);
                    e->del_item(n-1);
                    e->del_item(n-1);
                    n -= 2;
                }
                else {
                    it1 = Translate_item(handle,insn,&e->items[n-2],false);
                    it2 = Translate_item(handle,insn,&e->items[n-1],false);
                    sprintf(tmp,"%s%s%s",it1,op,it2);
                    list.push_back(tmp);
                    free(it2);
                    free(it1);
                    e->del_item(n-2);
                    e->del_item(n-2);
                    e->del_item(n-2);
                    n -= 3;
                }
                */
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
                /*
                it1 = Translate_var(handle,insn,e->items[n].item.name,true);
                lmem = (insn->detail->x86.operands[0].type ==X86_OP_MEM);
                if (list.size()>0) {
                    str = list.back();
                    list.pop_back();
                    if (lmem) {
                        sprintf(tmp,E_SET_MEM,ptr(insn->detail->x86.operands[0]),it1,str.c_str());
                    }
                    else {
                        sprintf(tmp,"%s = %s",it1,str.c_str());
                    }
                    strcat (buffer,tmp);
                }
                else {
                    it2 = Translate_item(handle,insn,&e->items[n-1],false);
                    if (lmem) {
                        sprintf(tmp,E_SET_MEM,ptr(insn->detail->x86.operands[0]),it1,it2);
                    }
                    else {
                        sprintf(tmp,"%s = %s",it1,it2);
                    }
                    strcat (buffer,tmp);
                    free(it2);
                }
                free(it1);
                */
                it1 = Translate_var(handle,insn,e->items[n].item.name,true);
                lmem = (insn->detail->x86.operands[0].type ==X86_OP_MEM);
                it2 = Translate_item(handle,insn,&e->items[n-1],false);
                if (lmem) {
                    sprintf(tmp,E_SET_MEM,ptr(insn->detail->x86.operands[0]),it1,it2);
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
            case _id_item::FUNCTION:
                /*
                it1 = Translate_var(handle,insn,e->items[n].item.name,false);
                if (list.size()>0) {
                    str = list.back();
                    list.pop_back();
                    sprintf(tmp,"%s%s%s",it1,str.c_str(),ENDF);
                    //strcat (buffer,tmp);
                    list.push_back(tmp);
                    e->del_item(n);
                    n -= 1;
                }
                else {
                    it2 = Translate_item(handle,insn,&e->items[n-1],false);
                    sprintf(tmp,"%s%s%s",it1,it2,ENDF);
                    //strcat (buffer,tmp);
                    list.push_back(tmp);
                    free(it2);
                    e->del_item(n-1);
                    e->del_item(n-1);
                    n -= 2;
                }
                free(it1);
                */
                it1 = Translate_var(handle,insn,e->items[n].item.name,false);
                it2 = Translate_item(handle,insn,&e->items[n-1],false);
                sprintf(tmp,"%s%s%s",it1,it2,ENDF);
                free(it2);
                free(it1);
                e->res_item(n,tmp);
                e->del_item(n-1);
                n -= 1;
                break;
            case _id_item::END:
                /*
                while (list.size()>0) {
                    str = list.back();
                    list.pop_back();
                    strcat (buffer,str.c_str());
                }
                strcat(buffer,ENDS);
                */
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
                if (ends) strcat(buffer,ENDS);
                break;
            default:
                break;
        }
        n++;
    }
    free(e);
    return (buffer);
}
