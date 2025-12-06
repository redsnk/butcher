// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format

#include "pe.h"

struct _PE *GetPE (char *name) {
FILE *f;
struct _PE *pe;
long l;

	f = fopen(name,"r");
	if (f!= NULL) {
		pe = malloc(sizeof(struct _PE));
		if (pe != NULL) {
			l = fread(&pe->ms_dos_stub,1,sizeof(struct _ms_dos_stub),f);
			if (l == sizeof(struct _ms_dos_stub)) {

			}
			free(pe);	
		}
		fclose(f);
	}
	return (NULL);
}

void FreePE (struct _PE *pe) {

}

char *GetMemoryPE (struct _PE *pe, int64_t offset, int64_t size) {

	return (NULL);
}
