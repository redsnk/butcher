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
    if (items != NULL) {
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

void Emit::del_item (int i) {
int n;

    if (i < count) {
        if (i != (count-1)) {
            for (n=i+1;n<count;n++) {
                items[n-1] = items[n];
            }
        }
        count--;
    }
}

void Emit::res_item (int i,char *str) {
    items[i].id = RESULT;
    strcpy (items[i].item.name,str);
}

void Emit::emit_item (enum _id_item id,const char *label) {
_s_item i;

    i.id = id;
    add_item(&i);
    if (debug) _emit((char *)"%s\n",label);  
}

void Emit::emit_item_number (enum _id_item id,const char *label,uint64_t num) {
_s_item i;

    i.id = id;
    i.item.num = num;
    add_item(&i);
    if (debug) _emit((char *)"%s = %i\n",label,num);  
}

void Emit::emit_item_name (enum _id_item id,const char *label,const char *name) {
_s_item i;

    i.id = id;
    strcpy (i.item.name,name);
    add_item(&i);
    if (debug) _emit((char *)"%s = '%s'\n",label,name);
}

void Emit::_emit(char *s, ...) {
va_list ap;

    va_start(ap, s);
    vfprintf(stdout, s, ap);
    va_end(ap);
}
