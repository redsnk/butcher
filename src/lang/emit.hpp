#ifndef _EMIT_H
#define _EMIT_H

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <string.h>
#include <stdint.h>

enum _id_item {
    RESULT,
    INDENT,
    NAME,
    NUMBER,
    FUNCTION,
    FUNC_VOID,
    EQUAL,
    NEQUAL,
    GT,
    LT,
    GTE,
    LTE,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    LIST,
    ASSIGN,
    END,
    LF,
    ENC,
    AND,
    OR,
    XOR,
    IFTHENELSE,
    IFTHEN
}; 

#define MAX_ITEM_NAME   (256)

union _v_item {
    uint64_t num;
    char name[MAX_ITEM_NAME];
};

struct _s_item {
    enum _id_item id;
    union _v_item item;
};

class Emit {
    public:
    int debug;
    int count;
    _s_item *items;

    void reset (void);
    void add_item (_s_item *i);
    void del_item (int i);
    void res_item (int i,char *str);
    void emit_item_number (enum _id_item id,const char *label,uint64_t num);
    void emit_item_name (enum _id_item id,const char *label,const char *name);
    void emit_item (enum _id_item id,const char *label);
    void _emit (char *s, ...);

    Emit();
    ~Emit();
};

#endif // _EMIT_H
