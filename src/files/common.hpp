#ifndef _COMMON_HP
#define _COMMON_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>

uint8_t *GetMemoryFile (FILE *f,uint64_t addr,uint64_t size,uint64_t v_addr,uint64_t v_size,uint64_t d_addr,uint64_t d_size,uint64_t *read);

#endif // _COMMON_H
