
// https://www.gnu.org/software/bison/manual/bison.pdf

#include <cstdlib>
#include <iostream>
#include <string>
#include "emit.hpp"
#include "scanner.hpp"
#include "parser.hpp"

int main(int argc, char **argv) {
YY_BUFFER_STATE b;
int n;
Emit *e;

	if (argc >= 2) {
	    yyscan_t scanner;
		yylex_init(&scanner);
		e = new Emit();
    	lang::Parser parser{ scanner,e };

		for (n=2;n<=argc;n++) {
			b = yy_scan_string (argv[n-1],scanner );
    		parser.parse();
			yy_delete_buffer (b,scanner);
		}

    	yylex_destroy(scanner);
		lang::_emit("done.\n");
	}
}
