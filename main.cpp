#include <stdio.h>
#include "src/butcher.hpp"
#include "src/arch/pe_x64.hpp"

int main (int argc, char **argv) {
    Butcher *b = new Pe_x64();
    uint8_t *text = b->Cut("/home/alex/src/butcher/samples/api.node",0x180002350); //0x180002240);
    if (text != NULL) {
        printf("%s\n",text);
    }
    delete b;
    return (0);
}
