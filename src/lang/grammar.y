%{
#include <iostream>
#include <string>
#include <cmath>
#include <stdarg.h>
#include "scanner.hpp"
%}
 
%require "3.3.2"
%language "C++"
%defines "parser.hpp"
%output "parser.cpp"
 
%define api.parser.class {Parser}
%define api.namespace {lang}
%define api.value.type variant
%param {yyscan_t scanner}

%code requires
{
	#include "emit.hpp"
}
 
%code provides
{
    #define YY_DECL \
        int yylex(lang::Parser::semantic_type *yylval, yyscan_t yyscanner)
    YY_DECL;

	namespace lang {
		void _emit(char *s, ...);
	}
}

%parse-param { Emit *emit }
 
%token <std::string> NAME
%token <long long>  INT
%token <double>     FLT
%token <char>       INTVAR FLTVAR
 
%nterm <long long>  iexp
%nterm <double>     fexp

%precedence         END
%precedence         '='
%precedence         EQUAL
%left               '+' '-' 
%left               MULTIPLY DIVIDE MODULO
%precedence         UMINUS
%precedence         FACTORIAL
%right              EXPONENT
%precedence			INDENT

%start stmt_list

%code
{
    namespace lang {
        void _emit(char *s, ...) {
		va_list ap;
        
			va_start(ap, s);
			vfprintf(stdout, s, ap);
			va_end(ap);
        }
    } // namespace lang
}
 
%%

expr: 
| NAME '(' expr ')'		{ emit->emit_func($1.c_str());}
| expr '+' expr			{ emit->emit_add(); }
| expr '-' expr			{ emit->emit_sub(); }
| expr MULTIPLY expr	{ emit->emit_mult(); }
| NAME '=' expr			{ emit->emit_assign($1.c_str());}
| expr EQUAL expr		{ emit->emit_equal();}
| NAME					{ emit->emit_name($1.c_str()); }
| INT					{ emit->emit_number($1); }
| END					{ emit->emit_end();}
| INDENT                { emit->emit_indent();}
;

stmt_list: expr
| stmt_list expr
;

%%

void lang::Parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}
