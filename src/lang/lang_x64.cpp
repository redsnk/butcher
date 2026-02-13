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

char *Lang_x64::get_op_str(csh handle,cs_x86_op op,int sign) {
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
            sprintf(buffer,(sign)?E_S_GET_MEM:E_GET_MEM,ptr(op),str);
            free(str);
            return (strdup(buffer));
        case X86_OP_IMM:
            sprintf(buffer,"0x%llx",op.imm);
            return (strdup(buffer));
    }
    return(NULL);
}

#define MAX_TRANS_BUFFER    (1024)

char *Lang_x64::Translate_var (csh handle,cs_insn *insn,char *name) {
    if (!strcmp(name,"op0")) {
        return (get_op_str(handle,insn->detail->x86.operands[0],false));
    }
    else if (!strcmp(name,"op1")) {
        return (get_op_str(handle,insn->detail->x86.operands[1],false));
    }
    else if (!strcmp(name,"sop0")) {
        return (get_op_str(handle,insn->detail->x86.operands[0],true));
    }
    else if (!strcmp(name,"sop1")) {
        return (get_op_str(handle,insn->detail->x86.operands[1],true));
    }
    return (strdup("<error>"));
}

char *Lang_x64::Translate_item (csh handle,cs_insn *insn,_s_item *i) {
char *buffer;

    switch (i->id) {
        case _id_item::NAME:
            return (Translate_var(handle,insn,i->item.name));
        case _id_item::NUMBER:
            buffer = (char *) malloc(MAX_TRANS_BUFFER);
            sprintf(buffer,"%lld",i->item.num);
            return (buffer);
        default:
            break;
    }
    return (strdup("<error>"));
}

char *Lang_x64::Translate (csh handle,char *s, cs_insn *insn) {
Emit *e;
char *buffer,*it1,*it2;
char tmp[MAX_TRANS_BUFFER];
int n;
std::vector<std::string> list;
std::string str;

    buffer = (char *) malloc(MAX_TRANS_BUFFER);
    buffer[0];
    e = Parse(s);
    n = 0;
    while (n < e->count) {
        switch (e->items[n].id) {
            case _id_item::INDENT:
                strcat (buffer,INDENT);
                break;
            case _id_item::ADD:
                it1 = Translate_item(handle,insn,&e->items[n-2]);
                it2 = Translate_item(handle,insn,&e->items[n-1]);
                sprintf(tmp,"%s + %s",it1,it2);
                free(it1);
                free(it2);
                list.push_back(tmp);
                break;
            case _id_item::ASSIGN:
                it1 = Translate_var(handle,insn,e->items[n].item.name);
                if (list.size()>0) {
                    str = list.back();
                    list.pop_back();
                    sprintf(tmp,"%s = %s",it1,str.c_str());
                    strcat (buffer,tmp);
                }
                else {
                    it2 = Translate_item(handle,insn,&e->items[n-1]);
                    sprintf(tmp,"%s = %s",it1,it2);
                    strcat (buffer,tmp);
                    free(it2);
                }
                free(it1);
                break;
            case _id_item::END:
                while (list.size()>0) {
                    str = list.back();
                    list.pop_back();
                    strcat (buffer,str.c_str());
                }
                break;
            default:
                break;
        }
        n++;
    }
    free(e);
    return (buffer);
}
