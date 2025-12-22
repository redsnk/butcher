#include <stdio.h>
#include "src/butcher.hpp"
#include "src/arch/pe_x64.hpp"

int main (int argc, char **argv) {
    Butcher *b = new Pe_x64();
    b->Cut("/home/alex/src/butcher/samples/api.node",0x180002350); //0x180002240);
    delete b;
    return (0);
}
