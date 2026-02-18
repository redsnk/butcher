#include "archive.hpp"

int Archive::Get_Address_At(uint64_t ptr,uint64_t *addr,int bits) {
uint8_t *mem;
uint64_t v,read;
int ret;

    ret = false;
    switch (bits) {
        case 32:
            mem = GetMemory(ptr,4,&read);
            if (mem != NULL) {
                if (read == 4) {
                    v = (((uint32_t ) mem[3]) << 24) | 
                        (((uint32_t ) mem[2]) << 16) | 
                        (((uint32_t ) mem[1]) << 8) | 
                        ((uint32_t ) mem[0]);
                    *addr = v;
                    ret = true;
                }
                free(mem);
            }
            break;
        case 64:
            mem = GetMemory(ptr,8,&read);
            if (mem != NULL) {
                if (read == 8) {
                    v = (((uint64_t ) mem[7]) << 56) | 
                        (((uint64_t ) mem[6]) << 48) | 
                        (((uint64_t ) mem[5]) << 40) | 
                        (((uint64_t ) mem[4]) << 32) | 
                        (((uint64_t ) mem[3]) << 24) | 
                        (((uint64_t ) mem[2]) << 16) | 
                        (((uint64_t ) mem[1]) << 8) | 
                        ((uint64_t ) mem[0]);
                    *addr = v;
                    ret = true;
                }
                free(mem);
            }
            break;
    }
    return (ret);
}