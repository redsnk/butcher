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
    sc->ret_bytes =0;
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
                if((subcodes[n].first >= sc->first) && (subcodes[n].last <= sc->last) && (subcodes[n].parent != SUBCODE_TOP)) {
                    // Old subcode included in this one and the old one has parent, replace it
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
//int lsi,lei,l;
//uint64_t end,send,saddress,of;
//uint8_t *nmem;

    for(int n=0;n<submem_count;n++) {
        if ((address >= submems[n].addr) && ((address+size) <= (submems[n].addr+submems[n].size))) {
            // Submem inside another.
            return;
        }
    }
    // TODO: DelSubMem
    // TODO: PackSubMem
    /*  
    end = address+size-1;
    for(int n=0;n<submem_count;n++) {
        saddress = submems[n].addr;
        send = saddress+submems[n].size-1;
        // start address included
        lsi = ((address >= saddress) && (address <= send));
        // end address included
        lei = ((end >= saddress) && (end <= send));
        if (lsi && lei) {
            // Submem inside exisiting submem, exit
            return;
        }
        if (lsi) {
            // Submem extends existing submem
            l = end-send;
            of = send-address;
            submems[n].mem = (uint8_t *) realloc(submems[n].mem,submems[n].size+l);
            memcpy(submems[n].mem+submems[n].size,mem+of,l);
            submems[n].size += l;
            return;
        }
        if (lei) {
            // Existing submem extends submem
            l = saddress-address;
            nmem = (uint8_t *) malloc(submems[n].size+l);
            memcpy(nmem+l,submems[n].mem,submems[n].size);
            memcpy(nmem,mem,l);
            free(submems[n].mem);
            submems[n].mem = nmem;
            submems[n].size += l;
            return;
        }
    }
    // New submem
    */
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

void Code::DelSubMem (int n) {
int m;

    if (submems[n].mem != NULL) {
        free(submems[n].mem);
    }
    for (m=n;m<(submem_count-1);m++) {
        submems[m] = submems[m+1];
    }
    submem_count--;
    submems = (struct _submem *) realloc(submems, sizeof(struct _submem)*(submem_count));
}

int Code::MixSubMems (int s1,int s2) {
int le1i,ls1i,le2i,ls2i;
uint64_t start1,start2,end1,end2,l,of;
    
    /*
    int ltest;
    uint64_t atest = 0x44fca9c;
    ltest = ((atest>=start1) && (atest<=end1)) | ((atest>=start2) && (atest<=end2));
    */

    start1 = submems[s1].addr;
    start2 = submems[s2].addr;
    end1 = start1+submems[s1].size-1;
    end2 = start2+submems[s2].size-1;
    ls1i = ((start1 >= start2) && (start1 <= end2));
    le1i = ((end1 >= start2) && end1 <= end2);
    ls2i = ((start2 >= start1) && (start2 <= end1));
    le2i = ((end2 >= start1) && end2 <= end1);
    if ((start1 == start2) && (end1 == end2)) {
        // Submem1 equal Submem2
        /*
        if (ltest) {
            ltest = true;
        }
        */
        DelSubMem(s2);
        return (true);
    }
    if (ls1i && le1i) {
        // Submem1 inside Submem2
        /*
        if (ltest) {
            ltest = true;
        }
        */
        DelSubMem(s1);
        return (true);
    }
    if (ls2i && le2i) {
        // Submem2 inside Submem1
        /*
        if (ltest) {
            ltest = true;
        }
        */
        DelSubMem(s2);
        return (true);
    }
    if (ls1i) {
        // Submem1 extends Submem2
        /*
        if (ltest) {
            ltest = true;
        }
        */
        l = end1-end2;
        of = end2-start1+1;
        submems[s2].mem = (uint8_t *) realloc(submems[s2].mem,submems[s2].size+l);
        memcpy(submems[s2].mem+submems[s2].size,submems[s1].mem+of,l);
        submems[s2].size += l;
        DelSubMem(s1);
        return (true);
    }
    if (ls2i) {
        // Submem2 extends Submem1
        /*
        if (ltest) {
            ltest = true;
        }
        */
        l = end2-end1;
        of = end1-start2+1;
        submems[s1].mem = (uint8_t *) realloc(submems[s1].mem,submems[s1].size+l);
        memcpy(submems[s1].mem+submems[s1].size,submems[s2].mem+of,l);
        submems[s1].size += l;
        DelSubMem(s2);
        return (true);
    }
    if ((end1+1) == start2) {
        // Submem2 continues Submem1
        /*
        if (ltest) {
            ltest = true;
        }
        */
        submems[s1].mem = (uint8_t *) realloc(submems[s1].mem,submems[s1].size+submems[s2].size);
        memcpy(submems[s1].mem+submems[s1].size,submems[s2].mem,submems[s2].size);
        submems[s1].size += submems[s2].size;
        DelSubMem(s2);
        return (true);
    }
    if ((end2+1) == start1) {
        // Submem1 continues Submem2
        /*
        if (ltest) {
            ltest = true;
        }
        */
        submems[s2].mem = (uint8_t *) realloc(submems[s2].mem,submems[s2].size+submems[s1].size);
        memcpy(submems[s2].mem+submems[s2].size,submems[s1].mem,submems[s1].size);
        submems[s2].size += submems[s1].size;
        DelSubMem(s1);
        return (true);
    }
    return (false);
}

void Code::PackSubMems(void) {
int n,m,lmix;
    
    do {
        lmix = false;
        for (n=0;(n<submem_count) && (!lmix);n++) {
            for (m=n+1;(m<submem_count) && (!lmix);m++) {
                if(MixSubMems(n,m)) {
                    lmix = true;
                }
            }
        }
    }
    while (lmix);
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

void Code::SetRetBytes (struct _subcode *sc,int bytes) {
struct _subcode *p;

    p = GetParent(sc);
    p->ret_bytes= bytes;
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

void Code::RenameFunction (struct _subcode *sc,char *name,int del) {
struct _subcode *p;

    p = GetParent(sc);
    if (del && (p->name != NULL)) {
        free (p->name);
        p->name = NULL;
    }
    if (p->name == NULL) {
        p->name = strdup(name);
    }
}

int Code::ExistFunctionName (char *name) {
int n;

    for (n=0;n<subcod_count;n++) {
        if ((subcodes[n].parent == SUBCODE_TOP) && (subcodes[n].name != NULL)) {
            if (strcmp(subcodes[n].name,name) == 0) {
                return (true);
            }
        }
    }
    return (false);
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

