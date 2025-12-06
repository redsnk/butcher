#include "global.h"
#include "src/pe/pe.h"

int main (int argc, char **argv) {
struct _PE *pe;

    pe = GetPE("/home/alex/src/butcher/samples/api.node");
    if (pe != NULL) {
		char *me = GetMemoryPE(pe,0,0);
		FreePE(pe);
	}
    return (0);
}
