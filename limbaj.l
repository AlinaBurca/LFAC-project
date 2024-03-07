%{
#include <stdio.h>
#include "limbaj.tab.h"
%}
%option noyywrap
%%
"int"|"float"|"char"|"string"|"bool"|"void" {yylval.value = strdup(yytext);return TYPE;}
"const"                                     {return CONST;}
"class"                                    { return CLASS; }
"public"|"private"|"protected"              { return ACCESS_MOD;}
"constructor"                                {return CONSTRUCTOR;}
"if"                                        { return IF; }
"else"                                      {  return ELSE; }
"while"                                     { return WHILE; }
"for"                                       { return FOR; }
"return"                                    { return RETURN;}
"true"|"false"                              {yylval.value = strdup(yytext); return BOOL_VALUE;}
"<="|"<"|">="|">"|"=="|"!="                {yylval.value = strdup(yytext);  return OPR;}
"&&"                                        {yylval.value = strdup(yytext); return AND;}
"||"                                        {yylval.value = strdup(yytext); return OR;}
"!"                                         { yylval.value = strdup(yytext); return NOT; }
"main"                                       {return MAIN;}
"Eval"                                        {return EVAL;}
"TypeOf"                                      {return TYPEOF;}
":="                                         {return ASSIGN;}
"array"                                      {return ARRAY;}
-?(0|([1-9][0-9]*))\.[0-9]+                    { yylval.value = strdup(yytext); return FLOAT_VALUE; }
0|(-?[1-9][0-9]*)                               { yylval.value = strdup(yytext); return INT_VALUE; }
[a-zA-Z|_][a-zA-Z0-9|_]*                        { yylval.value = strdup(yytext); return ID; }
\"[^\"]*\"                                     { yylval.value = strdup(yytext); return STRING_VALUE; }
\'[^\']*\'                                     { yylval.value = strdup(yytext); return CHAR_VALUE; }


[ \t] ;
\n {yylineno++;}
. {return yytext[0];}

 
