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

%precedence			LF
%precedence         END
%left				IF THEN ELSE
%precedence         '='
%left				LIST
%left               EQUAL NEQUAL LT GT LTE GTE
%left               '+' '-' B_AND B_OR B_XOR
%left               MUL DIV MOD
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
| NAME '(' expr ')'     { emit->emit_item_name(FUNCTION,"FUNCTION",$1.c_str()); }
| '(' expr ')'          { emit->emit_item(ENC,"ENC"); };
| expr LIST expr        { emit->emit_item(LIST,"LIST"); }
| expr '+' expr			{ emit->emit_item(ADD,"ADD"); }
| expr '-' expr			{ emit->emit_item(SUB,"SUB"); }
| expr B_AND expr       { emit->emit_item(AND,"AND"); }
| expr B_XOR expr       { emit->emit_item(XOR,"XOR"); }
| expr B_OR expr        { emit->emit_item(OR,"OR"); }
| expr MUL expr			{ emit->emit_item(MUL,"MUL"); }
| expr DIV expr         { emit->emit_item(DIV,"DIV"); }
| NAME '=' expr			{ emit->emit_item_name(ASSIGN,"ASSIGN",$1.c_str()); }
| expr EQUAL expr		{ emit->emit_item(EQUAL,"EQUAL"); }
| expr NEQUAL expr      { emit->emit_item(NEQUAL,"NEQUAL"); }
| expr LT expr          { emit->emit_item(LT,"LT"); }
| expr GT expr          { emit->emit_item(GT,"GT"); }
| expr LTE expr         { emit->emit_item(LTE,"LTE"); }
| expr GTE expr         { emit->emit_item(GTE,"GTE"); }
| NAME					{ emit->emit_item_name(NAME,"NAME",$1.c_str()); }
| INT					{ emit->emit_item_number(NUMBER,"NUMBER",$1); }
| END					{ emit->emit_item(END,"END"); }
| INDENT                { emit->emit_item(INDENT,"INDENT"); }
| LF                    { emit->emit_item(LF,"LF"); }
| IF expr THEN expr ELSE expr { emit->emit_item(IFTHENELSE,"IFTHENELS"); }
;

stmt_list: expr
| stmt_list expr
;

%%

void lang::Parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}
