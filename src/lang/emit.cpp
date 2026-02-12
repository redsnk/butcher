#include "emit.hpp"

Emit::Emit(void) {

}

void Emit::_emit(char *s, ...) {
va_list ap;

    va_start(ap, s);
    vfprintf(stdout, s, ap);
    va_end(ap);
}
