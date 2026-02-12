#include "language.hpp"

Emit *Language::Parse(char *str) {
YY_BUFFER_STATE b;
int n;
Emit *e;

    yyscan_t scanner;
    yylex_init(&scanner);
    e = new Emit();
    e->debug = true;                        // ******
    lang::Parser parser{ scanner,e };
    b = yy_scan_string (str,scanner );
    parser.parse();
    yy_delete_buffer (b,scanner);
    yylex_destroy(scanner);
    return (e);
}