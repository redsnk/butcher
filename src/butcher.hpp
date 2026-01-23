#ifndef _BUTCHER_H
#define _BUTCHER_H

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

struct _subcode {
    int id;
    char *name;
    int parent;
    uint64_t first;
    uint64_t last;
    cs_insn *insn;
    size_t count;
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
#define MAX_MEM_GETCODE (10*1024)

class Code {
    public:
        int next_id;
        struct _subcode *subcodes;
        int subcod_count;
        struct _submem *submems;
        int submem_count;
        uint64_t ep;
        //std::list<uint64_t> labels;
        std::set<uint64_t> labels;

        Code(uint64_t addr);
        ~Code();
        void NewSubCode (struct _subcode *sc);
        void AddSubcode (struct _subcode *sc);
        void AddSubMem (uint64_t address,uint8_t *mem,uint64_t size);
        int HasAddr (uint64_t addr,int parent);
        void Print (void);
};

class Archive {
    public:

};

class Butcher {
    public:
        csh handle;
        //
        virtual int CheckFile(char *file_name) = 0;
        virtual int OpenFile(char *file_name) = 0;
        virtual cs_err Cs_open(void) = 0;
        virtual void CloseFile(void) = 0;
        virtual uint8_t *GetMemory(uint64_t addr,uint64_t size,uint64_t *read) = 0;
        virtual int IsImportFunction (uint64_t addr, char *lib, char *func)= 0;
        virtual int IsSymbolFunction (uint64_t addr, char *func)= 0;
        virtual int IsSymbolObject (uint64_t addr, char *name)= 0;
        virtual int IsRet(cs_insn insn) = 0;
        virtual int IsCall(cs_insn insn, uint64_t *addr, char **name) = 0;
        virtual int IsJmp(cs_insn insn, uint64_t *addr) = 0;
        virtual int IsJcc(cs_insn insn, uint64_t *addr) = 0;
        virtual int IsInt(cs_insn insn, uint64_t *num) = 0;
        virtual int IsEnd(cs_insn *insn, int n, int count) = 0;
        virtual void PrintCodeC(Code *c) = 0;
        //
        int IsGroup (cs_insn insn, int group);
        Code *GetCode(Code *c,uint64_t address,char *name,int parent);
        void Cut(char *file_name,uint64_t address);
};

#endif // _BUTCHER_H