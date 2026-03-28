// https://www.capstone-engine.org/documentation.html
// https://www.capstone-engine.org/lang_c.html

#include "code.hpp"

Code::Code(uint64_t a) {
    next_id = SUBCODE_TOP+1;
    ep = a;
    subcod_count = 0;
    submem_count = 0;
}

void Code::NewSubCode (struct _subcode *sc) {
    sc->id = next_id++;
    sc->name = NULL;
    sc->labels = NULL;
    sc->l_count = 0;
    sc->anonjmp = false;
}

void Code::AddSubcode (struct _subcode *sc) {
    if (sc->parent != SUBCODE_TOP) {
        for (int n=0;n<subcod_count;n++) {
            if ((sc->parent == subcodes[n].id) || (sc->parent == subcodes[n].parent)) {
                // Parent or same parent
                if((sc->first >= subcodes[n].first) && (sc->last <= subcodes[n].last)) {
                    // Subcode included in the old one, discard it
                    return;
                }
                if((subcodes[n].first >= sc->first) && (subcodes[n].last <= sc->last)) {
                    // Old subcode included in this one, replace it
                    cs_free(subcodes[n].insn, subcodes[n].count);
                    subcodes[n] = *sc;
                    return;
                }
            }
        }
    }
    if (!subcod_count) {
        subcodes = (struct _subcode *) malloc(sizeof(struct _subcode));
    } else {
        subcodes = (struct _subcode *) realloc(subcodes, sizeof(struct _subcode)*(subcod_count+1));
    }
    subcodes[subcod_count++] = *sc;
}

void Code::AddSubMem (uint64_t address,uint8_t *mem,uint64_t size) {
    for(int n=0;n<submem_count;n++) {
        if ((submems[n].addr >= address) && ((submems[n].addr+submems[n].size) <= (address+size))) {
            return;
        }
    }
    if (!submem_count) {
        submems = (struct _submem *) malloc(sizeof(struct _submem));
    } else {
        submems = (struct _submem *) realloc(submems, sizeof(struct _submem)*(submem_count+1));
    }
    if (mem != NULL) {
        submems[submem_count].mem = (uint8_t *) malloc(size);
        memcpy(submems[submem_count].mem,mem,size);
    }
    else {
        submems[submem_count].mem = NULL;
    }
    submems[submem_count].size = size;
    submems[submem_count].addr = address;
    submem_count++;
}

int Code::HasAddr (uint64_t addr,int parent) {
    if (parent != SUBCODE_TOP) {
        // jmp
        for (int n=0;n<subcod_count;n++) {
            if ((parent == subcodes[n].id) || (parent == subcodes[n].parent)) {
                if ((addr >= subcodes[n].first) && (addr <= subcodes[n].last)) {
                    return (true);
                }
            }
        }
    }
    else {
        // call
        for (int n=0;n<subcod_count;n++) {
            if ((subcodes[n].parent == SUBCODE_TOP) && (addr == subcodes[n].first)) {
                return (true);
            }
        }
    }
    return (false);
}

void Code::Print (void) {
int n,i;

    for (n=0;n<subcod_count;n++) {
        printf("==== SC(%i) ====\n",n);
        for (i=0;i<subcodes[n].count;i++) {
            printf("0x%llx:\t%s\t\t%s\n", subcodes[n].insn[i].address, subcodes[n].insn[i].mnemonic,subcodes[n].insn[i].op_str);
            if (subcodes[n].insn[i].address == subcodes[n].last) {
                break;
            }
        }
    }
}

struct _subcode *Code::GetParent(struct _subcode *sc) {
int n;

    if (sc->parent != SUBCODE_TOP) {
        for (n=0;n<subcod_count;n++) {
            if (subcodes[n].id == sc->parent) {
                return (&subcodes[n]);
            }
        }
    }
    return (sc);
}

void Code::AddLabel (struct _subcode *sc,uint64_t addr) {
struct _subcode *p;

    p = GetParent(sc);
    if (!ExistLabel(p,addr)) {
        if (!p->l_count) {
            p->labels = (uint64_t *) malloc(sizeof(uint64_t));
        }
        else {
            p->labels = (uint64_t *) realloc(p->labels,sizeof(uint64_t)*(p->l_count+1));
        }
        p->labels[p->l_count++] = addr;
    }
}

int Code::ExistLabel (struct _subcode *sc,uint64_t addr) {
int n;
struct _subcode *p;

    p = GetParent(sc);
    for (n=0;n<p->l_count;n++) {
        if (p->labels[n] == addr) {
            return (true);
        }
    }
    return (false);
}

void Code::SetAnonJmp (struct _subcode *sc) {
struct _subcode *p;

    p = GetParent(sc);
    p->anonjmp = true;
}

char *Code::GetFunctionName (uint64_t addr) {
int n;

    for (n=0;n<subcod_count;n++) {
        if ((subcodes[n].parent == SUBCODE_TOP) && (subcodes[n].first == addr) && (subcodes[n].name != NULL)) {
            return (strdup(subcodes[n].name));
        }
    }
    return (NULL);
}

Code::~Code() {
int n;

    if (subcod_count > 0) {
        for (n=0;n<subcod_count;n++) {
            cs_free(subcodes[n].insn, subcodes[n].count);
            if (subcodes[n].name != NULL) {
                free(subcodes[n].name);
            }
            if (subcodes[n].labels != NULL) {
                free(subcodes[n].labels);
            }
        }
        free(subcodes);
    }
    if (submem_count > 0) {
        for (n=0;n<submem_count;n++) {
            if(submems[n].mem != NULL) {
                free(submems[n].mem);
            }
        }
        free(submems);
    }
}

