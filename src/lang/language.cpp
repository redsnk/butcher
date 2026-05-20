#include "language.hpp"

void Language::PrintF(const char *format,...) {
char buffer[MAX_PRINTF];
int n;

    va_list argptr;
    va_start(argptr, format);
    //vsprintf(buffer,format, argptr);
    n = vsnprintf(buffer,MAX_PRINTF,format,argptr);
    if (n == (MAX_PRINTF-1)) {
        fprintf(stderr,"Error: PrintF/vsnprintf not enough\n");
        exit(0);
    }
    va_end(argptr);
    if (lstdout) {
        // STDOUT
        fwrite(buffer,1,strlen(buffer),stdout);
    }
    else {
        // BUFFER
        if (out == NULL) {
            out = (char *) calloc(BUFFER_STEP,1);
            len = 0;
            max_len = BUFFER_STEP;
        }
        if (strlen(buffer) > (max_len-len-1)) {
            out = (char *) realloc(out,max_len+BUFFER_STEP);
            max_len += BUFFER_STEP;
        }
        strcat(out,buffer);
        len += strlen(buffer);
    }
}

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