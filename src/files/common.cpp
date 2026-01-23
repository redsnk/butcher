#include "common.hpp"

/*
uint8_t *GetMemoryFile (FILE *f,uint64_t addr,uint64_t size,uint64_t v_addr,uint64_t v_size,uint64_t d_addr,uint64_t d_size,uint64_t *read) {
uint64_t start, end, offset;
uint8_t *m;
long l;

    start = v_addr;
    end = start + v_size;
    if ((addr >= start) && (addr < end)) {
        // addr inside the virtual section
        if ((end-addr) < size) {
            // not enought size
            size = end-addr-1;
            if (!size) {
                // size == 0
                return (NULL);
            }
        }
        m = (uint8_t *) malloc(size);
        if (m != NULL) {
            offset = d_addr+(addr-start);
            l = fseek(f,offset,SEEK_SET);
            if (l != -1) {
                l = fread(m,1,size,f);
                if (l == size) {
                   *read = size;
                    return (m);
                } else {
                    fprintf(stderr,"GetMemoryFile error: fread\n");
                }
            } else {
                fprintf(stderr,"GetMemoryFile error: fseek\n");
            }
            free(m);		
        }
        else {
            fprintf(stderr,"GetMemoryFile error: malloc\n");
        }
    }
    return (NULL);
}
*/

uint8_t *GetMemoryFile (FILE *f,uint64_t addr,uint64_t size,uint64_t v_addr,uint64_t v_size,uint64_t d_addr,uint64_t d_size,uint64_t *read) {
uint64_t last;
uint8_t *m;
long l,c;

    last = addr + size - 1;
    if ((addr >= v_addr) && (addr < (v_addr + v_size))) {
        // addr inside the virtual section
        if ((last >= (v_addr + v_size))) {
            // last outside the virtual section
            last = v_addr + v_size - 1;
        }
        // last inside the virtual section
        *read = last - addr + 1;
        m = (uint8_t *) malloc(*read);
        if (m != NULL) {
            memset(m,0,size);
            if ((addr - v_addr  + 1) <= d_size) {
                // addr inside the file section
                if ((last - v_addr + 1) <= d_size) {
                    // last inside the file section
                    c = *read;
                }
                else {
                    // last outside the file section
                    c = v_addr + v_size - addr;
                }
            }
            else {
                c = 0;
            }
            if (c) {
                l = fseek(f,d_addr+(addr-v_addr),SEEK_SET);
                if (l != -1) {
                    l = fread(m,1,c,f);
                    if (l == c) {
                        return (m);
                    } else {
                        fprintf(stderr,"GetMemoryFile error: fread\n");
                    }
                } else {
                    fprintf(stderr,"GetMemoryFile error: fseek\n");
                }
            }
            else {
                return(m);
            }
            free(m);
        }
        else {
            fprintf(stderr,"GetMemoryFile error: malloc\n"); 
        }
    }
    return (NULL);
}
