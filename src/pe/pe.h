#ifndef _PE_H
#define _PE_H

#include "../../global.h"

struct _ms_dos_stub {
	unsigned char head[0x3c-1];
	int32_t off_signature;
};

struct _PE {
	struct _ms_dos_stub ms_dos_stub;
};

struct _PE *GetPE (char *name);
void FreePE (struct _PE *pe);
char *GetMemoryPE (struct _PE *pe, int64_t offset, int64_t size);

#endif	// _PE_H
