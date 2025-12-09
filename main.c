#include <stdio.h>
#include "src/butcher.h"

int main (int argc, char **argv) {
    char *text = Cut("/home/alex/src/butcher/samples/api.node",0x180002240,"test_func");
    if (text != NULL) {
        printf("%s\n",text);
    }
    return (0);
}
