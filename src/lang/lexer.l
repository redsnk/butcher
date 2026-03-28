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

"if"        return Parser::token::IF;
"then"      return Parser::token::THEN;
"else"      return Parser::token::ELSE;
"fi"        return Parser::token::FI;
"goto"      return Parser::token::GOTO;
{name}      yylval->emplace<std::string>(yytext); return Parser::token::NAME; 
{integer}   yylval->emplace<long long>(strtoll(yytext, nullptr, 10)); return Parser::token::INT;
{float}     yylval->emplace<double>(strtod(yytext, nullptr)); return Parser::token::FLT;
"*"         return Parser::token::MUL;
"/"         return Parser::token::DIV;
"%"         return Parser::token::MOD;
"!"         return Parser::token::NOT;
"$"         return Parser::token::EXPONENT;
"=="        return Parser::token::EQUAL;
"!="        return Parser::token::NEQUAL;
"<<"        return Parser::token::SHL;
">>"        return Parser::token::SHR;
"<"         return Parser::token::LT;
">"         return Parser::token::GT;
"<="        return Parser::token::LTE;
">="        return Parser::token::GTE;
"º"         return Parser::token::END;
"."         return Parser::token::INDENT;
","			return Parser::token::LIST;
":"         return Parser::token::LF;
"&"         return Parser::token::B_AND;
"|"         return Parser::token::B_OR;
"^"         return Parser::token::B_XOR;
";"         return Parser::token::JOIN;
" "         /* ignore space */
.           return yytext[0];
 
%%
 
