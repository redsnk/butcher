#ifndef _EMIT_H
#define _EMIT_H

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <string.h>
#include <stdint.h>

enum _id_item {
    INDENT,
    NAME,
    NUMBER,
    FUNCTION,
    EQUAL,
    ADD,
    SUB,
    MULT,
    ASSIGN,
    END
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
    void emit_name (const char *name);
    void emit_number (uint64_t num);
    void emit_func (const char *name);
    void emit_assign (const char *name);
    void emit_equal (void);
    void emit_add (void);
    void emit_sub (void);
    void emit_mult (void);
    void emit_end (void);
    void emit_indent (void);
    void _emit (char *s, ...);

    Emit();
    ~Emit();
};

#endif // _EMIT_H
