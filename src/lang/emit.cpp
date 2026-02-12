#include "emit.hpp"

Emit::Emit(void) {
    items = NULL;
    count = 0;
    debug = false;
}

Emit::~Emit() {
    reset();
}

void Emit::reset (void) {
    if (count) {
        free(items);
        count = 0;
    }
}

void Emit::add_item (_s_item *i) {
    if (!count) {
        items = (_s_item *) malloc(sizeof(_s_item));
    }
    else {
        items = (_s_item *) realloc(items,sizeof(_s_item)*(count+1));
    }
    items[count++] = *i;
}

void Emit::emit_name (const char *name) {
_s_item i;

    i.id = NAME;
    strcpy (i.item.name,name);
    add_item(&i);
    if (debug) _emit((char *)"NAME = '%s'\n",name);
}

void Emit::emit_number (uint64_t num) {
_s_item i;

    i.id = NUMBER;
    i.item.num = num;
    add_item(&i);
    if (debug) _emit((char *)"NUMBER = %i\n",num);
}

void Emit::emit_func (const char *name) {
_s_item i;

    i.id = FUNCTION;
    strcpy (i.item.name,name);
    add_item(&i);
    if (debug) _emit((char *)"FUNCTION = '%s'\n",name);
}

void Emit::emit_assign (const char *name) {
_s_item i;

    i.id = ASSIGN;
    strcpy (i.item.name,name);
    add_item(&i);
    if (debug) _emit((char *)"ASSIGN = '%s'\n",name);
}

void Emit::emit_equal (void) {
_s_item i;

    i.id = EQUAL;
    add_item(&i);
    if (debug) _emit((char *)"EQUAL\n");
}

void Emit::emit_add (void) {
_s_item i;

    i.id = ADD;
    add_item(&i);
    if (debug) _emit((char *)"ADD\n");
}

void Emit::emit_sub (void) {
_s_item i;

    i.id = SUB;
    add_item(&i);
    if (debug) _emit((char *)"SUB\n");
}

void Emit::emit_end (void) {
_s_item i;

    i.id = END;
    add_item(&i);
    if (debug) _emit((char *)"END\n");
}

void Emit::emit_indent (void) {
_s_item i;

    i.id = INDENT;
    add_item(&i);
    if (debug) _emit((char *)"INDENT\n");
}

void Emit::emit_mult (void) {
_s_item i;

    i.id = MULT;
    add_item(&i);
    if (debug) _emit((char *)"MULT\n");
}

void Emit::_emit(char *s, ...) {
va_list ap;

    va_start(ap, s);
    vfprintf(stdout, s, ap);
    va_end(ap);
}
