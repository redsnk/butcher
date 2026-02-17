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

"true"      return Parser::token::TRUE;
"false"     return Parser::token::FALSE;
{name}      yylval->emplace<std::string>(yytext); return Parser::token::NAME; 
{integer}   yylval->emplace<long long>(strtoll(yytext, nullptr, 10)); return Parser::token::INT;
{float}     yylval->emplace<double>(strtod(yytext, nullptr)); return Parser::token::FLT;
"*"         return Parser::token::MULTIPLY;
"/"         return Parser::token::DIVIDE;
"%"         return Parser::token::MODULO;
"!"         return Parser::token::FACTORIAL;
"^"         return Parser::token::EXPONENT;
"=="        return Parser::token::EQUAL;
"!="        return Parser::token::NEQUAL;
"<"         return Parser::token::LT;
">"         return Parser::token::GT;
"<="        return Parser::token::LTE;
">="        return Parser::token::GTE;
";"         return Parser::token::END;
"."         return Parser::token::INDENT;
","			return Parser::token::LIST;
"|"         return Parser::token::LF;
[-+()=]     return yytext[0];
.           /* no action on unmatched input */
 
%%
 
