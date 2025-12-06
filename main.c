#include "global.h"
#include "src/pe/pe.h"

int main (int argc, char **argv) {
struct _PE *pe;

    pe = GetPE("/home/alex/src/butcher/samples/api.node");
    if (pe != NULL) {
		uint8_t *m = GetMemoryPE(pe,0x180002240,0x400);
        if (m != NULL) {
            free(m);
        }
		FreePE(pe);
	}
    return (0);
}
