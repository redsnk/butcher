%{
#include <cstdlib>
#include "parser.hpp"
using namespace lang;
%}
 
%option reentrant interactive noyywrap noyylineno nodefault outfile="scanner.cpp" header="scanner.hpp"

name            ([A-Za-z][A-Za-z0-9_]*)
dseq            ([[:digit:]]+)
dseq_opt        ({dseq}?)
frac            (({dseq_opt}"."{dseq})|{dseq}".")
exp             ([eE][+-]?{dseq})
exp_opt         ({exp}?)
integer         ({dseq})
float           (({frac}{exp_opt})|({dseq}{exp}))
 
%%

{name}      yylval->emplace<std::string>(yytext); return Parser::token::NAME; 
{integer}   yylval->emplace<long long>(strtoll(yytext, nullptr, 10)); return Parser::token::INT;
{float}     yylval->emplace<double>(strtod(yytext, nullptr)); return Parser::token::FLT;
"*"         return Parser::token::MULTIPLY;
"/"         return Parser::token::DIVIDE;
"%"         return Parser::token::MODULO;
"!"         return Parser::token::FACTORIAL;
"^"         return Parser::token::EXPONENT;
"=="        return Parser::token::EQUAL;
";"         return Parser::token::END;
[-+()=]     return yytext[0];
.           /* no action on unmatched input */
 
%%
 
