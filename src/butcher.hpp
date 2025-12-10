#ifndef _BUTCHER_H
#define _BUTCHER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <capstone/capstone.h>

class Butcher {
    public:
        csh handle;

        virtual int OpenFile(char *file_name) = 0;
        virtual cs_err Cs_open(void) = 0;
        virtual void CloseFile(void) = 0;
        virtual uint8_t *GetMemory(uint64_t addr,uint64_t size) = 0;
        // 
        uint8_t *Cut(char *file_name,uint64_t address,char *func_name);
};

#endif // _BUTCHER_H