#ifndef _CODE_H
#define _CODE_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <list>
#include <set>
#include <algorithm>
#include <cstring>
#include <capstone/capstone.h>

#define SUBCODE_TOP     0

#define REG_NONE        0
#define REG_USED        1
#define REG_UPDATED     2

struct _subcode {
    int id;
    char *name;
    int parent;
    uint64_t first;
    uint64_t last;
    cs_insn *insn;
    size_t count;
    //std::set<uint64_t> labels;
    uint64_t *labels;
    size_t l_count;
    int anonjmp;
    int ret_bytes;
    int regs[X86_REG_ENDING];
};

struct _submem {
    uint64_t addr;
    uint64_t size;
    uint8_t *mem;
};

struct _call {
    uint64_t addr;
    char *name;  
};

#define INIT_MEM_GETCODE (1024)
#define STEP_MEM_GETCODE (1024)
#define MAX_MEM_GETCODE (1024*1024)

class Code {
    public:
        int next_id;
        struct _subcode *subcodes;
        int subcod_count;
        struct _submem *submems;
        int submem_count;
        uint64_t ep;
        //std::set<uint64_t> labels;

        Code(uint64_t addr);
        ~Code();
        void NewSubCode (struct _subcode *sc);
        void AddSubcode (struct _subcode *sc);
        void AddSubMem (uint64_t address,uint8_t *mem,uint64_t size);
        void DelSubMem (int n);
        int MixSubMems (int s1,int s2);
        void PackSubMems(void);
        int HasAddr (uint64_t addr,int parent);
        void Print (void);
        struct _subcode *GetParent(struct _subcode *sc);
        void AddLabel (struct _subcode *sc,uint64_t addr);
        int ExistLabel (struct _subcode *sc,uint64_t addr);
        void SetAnonJmp (struct _subcode *sc);
        void SetRetBytes (struct _subcode *sc,int bytes);
        char *GetFunctionName (uint64_t addr);
        int ExistFunctionName (char *name);
        void RenameFunction (struct _subcode *sc,char *name,int del);
};

#endif // _CODE_H