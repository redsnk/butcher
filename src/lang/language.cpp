#include "language.hpp"

char *Language::Indent(char *str) {
char *p,*n;
char *res;

    res = (char *) malloc(MAX_STR_OP);
    res[0] = 0;
    p = str;
    while (strlen(p)) {
        n = strchr(p,'\n');
        if (n != NULL) {
            *n = 0;
            strcat(res,INDENT());
            strcat(res,p);
            strcat(res,"\n");
            p = n + 1;
        }
        else {
            strcat(res,INDENT());
            strcat(res,p);
            break;
        }
    }
    return (res);
}

Emit *Language::Parse(char *str) {
YY_BUFFER_STATE b;
int n;
Emit *e;

    yyscan_t scanner;
    yylex_init(&scanner);
    e = new Emit();
    //e->debug = true;                        // ******
    lang::Parser parser{ scanner,e };
    b = yy_scan_string (str,scanner );
    parser.parse();
    yy_delete_buffer (b,scanner);
    yylex_destroy(scanner);
    return (e);
}