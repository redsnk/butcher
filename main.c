#include "global.h"
#include "src/pe/pe.h"
#include "src/butcher/butcher.h"

int main (int argc, char **argv) {
struct _PE *pe;

    pe = GetPE("/home/alex/src/butcher/samples/api.node");
    if (pe != NULL) {
        char *text = Cut(pe,0x180002240);
        if (text != NULL) {
            printf("%s\n",text);
        }
		FreePE(pe);
	}
    return (0);
}
