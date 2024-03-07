%{
#include <iostream>
#include <vector>
#include<cstring>
#include<string>
#include "IdList.h"
#include "Structs.h"
extern FILE* yyin;
extern char* yytext;
extern int yylineno;
extern int yylex();
void yyerror(const char * s);
class IdList ids;

extern FILE* yyin;
extern char* yytext;
char def_func_var[30];  //retinem numele functiei curente pentru variabila (clasa, global, nume_functie_delcarata_global)
char def_from_func[30]; //retinem in ce parte a codului a fost declarata o functie (clasa, global) pentru 
char nume_fct[30]; //retinem numele functiei pentru tabelul de functii


%}
%union {
     char* value;
     struct exprInfo *exprstr; 

}
%token  ASSIGN CONST CLASS ACCESS_MOD ARRAY IF ELSE WHILE FOR RETURN MAIN 
%token  EVAL TYPEOF CONSTRUCTOR
%token<value> TYPE ID INT_VALUE FLOAT_VALUE CHAR_VALUE STRING_VALUE BOOL_VALUE OR AND NOT OPR
%type<exprstr> expr


%left OPR
%left '+' '-'
%left NOT
%left '^' '*' '/' '%' 
%left '(' ')'
%left AND OR
%left ','

%start progr
%%


progr: class_section global_variables global_functions main {printf("The programme is correct!\n");}

global_variables: global_variables decl_variable
                | {strcpy(def_func_var, "global"); strcpy(def_from_func, "global");} decl_variable
                ;


global_functions: global_functions function 
                | function
                ;


function: TYPE ID '(' {strcpy(nume_fct,$2); strcpy(def_func_var, $2); ids.addFct($2,$1,"","global",false);} list_param ')'  
                  '{' func_body '}' 

         | TYPE ID '(' { strcpy(nume_fct,$2); strcpy(def_func_var, $2); ids.addFct($2,$1,"","global",false);}')' 
                   '{' func_body '}'

         | CONST TYPE ID '('{ strcpy(nume_fct,$3); strcpy(def_func_var, $3); ids.addFct($3,$2, "","global",true);} list_param ')'  
                          '{' func_body '}' 

         | CONST TYPE ID '(' { strcpy(nume_fct,$3); strcpy(def_func_var, $3); ids.addFct($3,$2,"","global",true);}')' 
                         '{' func_body '}'
         ;

list_param : param 
            | list_param ','  param 
            ;



param : TYPE ID         {ids.parFct($1,$2,nume_fct);  ids.addVar($2, $1, "", def_func_var, false);}
      | TYPE ID '[' ']' {ids.parFct($1,$2,nume_fct);  ids.addVar($2, $1, "", def_func_var, false); }
      ; 

func_body: func_body statement
         | statement
         ;

statement: decl_variable    //ne putem declara variabile si in functii (deci putem avea nume de variabile la fel in global si main)
         | update_variable 
         | if_statement
         | while_statement
         | for_statement
         | return_statement ';'
         | ID '(' list_param_call ')' ';'   {ids.isNotDefinedFct($1); }
         | ID '(' ')' ';'                   {ids.isNotDefinedFct($1); }
         | eval_type ';'
         | apel_obj ';'
         ;

eval_type : EVAL '(' expr ')' 
          | TYPEOF '(' expr ')' {char a[50], expr[50]; if(!ids.exprOk(ids.typeOf($3->str, def_func_var))) ids.typeOfError(ids.typeOf($3->str, def_func_var)); else {strcpy(a, ids.typeOf($3->str, def_func_var).c_str()); strcpy(expr, $3->str); printf("\n\nTypeOf(%s): %s\n\n", expr, a);}}
          ;


               
update_variable: ID ASSIGN expr ';'       { ids.isNotDefinedVar($1, def_func_var); ids.verifExpr(ids.typeOf($3->str, def_func_var)); if(ids.exprOk(ids.typeOf($3->str, def_func_var))) {ids.verifTypeVar($1, def_func_var, ids.typeOf($3->str, def_func_var));} }
               | CONST ID ASSIGN expr ';' { ids.isNotDefinedVar($2, def_func_var); ids.verifExpr(ids.typeOf($4->str, def_func_var)); if(ids.exprOk(ids.typeOf($4->str, def_func_var))) {ids.verifTypeVar($2, def_func_var, ids.typeOf($4->str, def_func_var));} }
               | ID '[' ID ']'            {char n[10]; strcpy(n, $1); strcat(n, "["); ids.isNotDefinedVar(n, def_func_var); ids.isNotDefinedVar($3, def_func_var); } ASSIGN expr ';' { char n[10]; strcpy(n, $1); strcat(n, "["); ids.verifExpr(ids.typeOf($7->str, def_func_var)); if(ids.exprOk(ids.typeOf($7->str, def_func_var))) {ids.verifTypeVar(n, def_func_var, ids.typeOf($7->str, def_func_var));}}
               | ID '['INT_VALUE']'       {char n[10]; strcpy(n, $1); strcat(n, "["); ids.isNotDefinedVar(n, def_func_var); } ASSIGN expr ';' { char n[10]; strcpy(n, $1); strcat(n, "["); ids.verifExpr(ids.typeOf($7->str, def_func_var)); if(ids.exprOk(ids.typeOf($7->str, def_func_var))) {ids.verifTypeVar(n, def_func_var, ids.typeOf($7->str, def_func_var));}}
               ;


decl_variable : TYPE ID ';'                                { ids.addVar($2, $1, "", def_func_var, false); }
               | TYPE ID ASSIGN expr ';'                   { ids.addVar($2, $1, "", def_func_var, false); ids.verifExpr(ids.typeOf($4->str, def_func_var)); if(ids.exprOk(ids.typeOf($4->str, def_func_var))){ids.verifTypeVar($2, def_func_var, ids.typeOf($4->str, def_func_var));}  }
               | CONST TYPE ID ASSIGN expr ';'             { ids.addVar($3, $2, "", def_func_var, true);  ids.verifExpr(ids.typeOf($5->str, def_func_var)); if(ids.exprOk(ids.typeOf($5->str, def_func_var))){ids.verifTypeVar($3, def_func_var, ids.typeOf($5->str, def_func_var));}}
               | TYPE ID '[' INT_VALUE']' ';'              { char s[10]; sprintf(s, "%s[%s]", $2, $4); ids.addVar(s, $1, "", def_func_var, false);}
               | CONST TYPE ID '[' INT_VALUE']' ';'        { char s[10]; sprintf(s, "%s[%s]", $3, $5); ids.addVar(s, $2, "", def_func_var, true);}
               | ID ID ';'                                 { ids.verifClassObj($1); ids.addVar($2, $1, "", def_func_var, false);}
               | ID ID ASSIGN expr ';'                     { ids.verifClassObj($1); ids.addVar($2, $1, "", def_func_var, false); ids.verifExpr(ids.typeOf($4->str, def_func_var)); if(ids.exprOk(ids.typeOf($4->str, def_func_var))){ids.verifTypeVar($2, def_func_var, ids.typeOf($4->str, def_func_var));}}
               ;


apel_obj : ID '.' ID ASSIGN expr       {ids.isNotDefinedVar($1, def_func_var); ids.isNotDefinedVar($3, def_func_var); ids.verifExpr(ids.typeOf($5->str, def_func_var)); if(ids.exprOk(ids.typeOf($5->str, def_func_var))){ids.verifTypeVar($3, def_func_var, ids.typeOf($5->str, def_func_var));}}
          ;


if_statement: IF '(' expr ')' '{' func_body '}' ELSE '{' func_body '}'{ids.verifExpr(ids.typeOf($3->str, def_func_var));}
            | IF '(' expr ')' '{' func_body '}'                       {ids.verifExpr(ids.typeOf($3->str, def_func_var));}
            ;

while_statement: WHILE '(' expr {ids.verifExpr(ids.typeOf($3->str, def_func_var));}')' '{' func_body '}'
               ;

for_statement: FOR '(' TYPE ID {ids.addVar($4, $3, "", def_func_var, false);} ASSIGN expr {ids.verifExpr(ids.typeOf($7->str, def_func_var));}';' expr {ids.verifExpr(ids.typeOf($10->str, def_func_var));}';' ID ASSIGN expr {ids.verifExpr(ids.typeOf($15->str, def_func_var));  ids.isNotDefinedVar($13, def_func_var);}')' '{' func_body'}'


return_statement: RETURN expr {ids.verifExpr(ids.typeOf($2->str, def_func_var));}
                | RETURN 
 
                ;
//expresii aritmetice si booleene


expr : expr '+' expr                       {char* a = (char *)malloc(10); sprintf(a, "%s+%s", $1->str, $3->str); /*printf("Expresia este: %s\n", a);*/ $$->str=a;}
      | expr '-' expr                      {char* b = (char *)malloc(10); sprintf(b, "%s-%s", $1->str, $3->str);/* printf("Expresia este: %s\n", b);*/ $$->str=b;}
      | expr '*' expr                      {char* c = (char *)malloc(100); sprintf(c, "%s*%s", $1->str, $3->str); /*printf("Expresia este: %s\n", c); */$$->str=c;}
      | expr '/'  expr                     {char* d = (char *)malloc(100); sprintf(d, "%s/%s", $1->str, $3->str); /*printf("Expresia este: %s\n", d); */$$->str=d;}
      | expr '%' expr                      {char* e = (char *)malloc(100); sprintf(e, "%s%%%s", $1->str, $3->str); /*printf("Expresia este: %s\n", e); */$$->str=e;}
      | '(' expr ')'                       {$$ = $2;  char* m = (char *)malloc(10); sprintf(m, "%s", $2->str); $$->str = m;}
      | expr AND expr                      {char* f = (char *)malloc(100); sprintf(f, "%s %s %s", $1->str, $2, $3->str);/* printf("Expresia este: %s\n", f);*/ $$->str=f;}
      | expr OR expr                       {char* g = (char *)malloc(100); sprintf(g, "%s %s %s", $1->str, $2, $3->str); /*printf("Expresia este: %s\n", g); */$$->str=g;}
      | NOT expr                           {char* h = (char *)malloc(100); sprintf(h, "%s%s", $1, $2->str); /*printf("Expresia este: %s\n", h); */$$->str=h;}
      | expr OPR expr                      {char* i = (char *)malloc(100); sprintf(i, "%s %s %s", $1->str, $2, $3->str); /*printf("Expresia este: %s\n", i);*/ $$->str=i;}
      | ID                                 {ids.isNotDefinedVar($1, def_func_var); $$->str=$1; }
      | ID '[' ID ']'                      { char n[10]; strcpy(n, $1); strcat(n, "["); ids.isNotDefinedVar(n, def_func_var); ids.isNotDefinedVar($3, def_func_var); char* j = (char *)malloc(10); sprintf(j, "%s[%s]", $1, $3); $$->str=j;}
      | ID '[' INT_VALUE ']'               { char n[10]; strcpy(n, $1); strcat(n, "["); ids.isNotDefinedVar(n, def_func_var); char* k = (char *)malloc(10); sprintf(k, "%s[%s]", $1, $3); $$->str=k;}
      | ID '['']'                          {char n[10]; strcpy(n, $1); strcat(n, "["); ids.isNotDefinedVar(n, def_func_var); char* l = (char *)malloc(10); sprintf(l, "%s[ ]", $1); $$->str=l;}
      | BOOL_VALUE                         { ids.addConstant($1, "bool", $1, false); $$->str=$1;}
      | INT_VALUE                          { ids.addConstant($1, "int", $1, false); $$->str=$1;}
      | FLOAT_VALUE                        { ids.addConstant($1, "float", $1, false); $$->str=$1;}
      | STRING_VALUE                       { ids.addConstant($1, "string", $1, false); $$->str=$1;}
      | CHAR_VALUE                         { ids.addConstant($1, "char", $1, false); $$->str=$1;}
      | ID '(' list_param_call ')'         {ids.isNotDefinedFct($1);  $$->str=$1; }
      | ID '(' ')'                         {ids.isNotDefinedFct($1); ids.isNotDefinedFct($1); $$->str=$1; }
      | ID '.' ID '(' list_param_call ')'  {ids.isNotDefinedVar($1, def_func_var); ids.isNotDefinedFct($3); $$->str=$3; }
      | ID '.' ID '(' ')'                  {ids.isNotDefinedVar($1, def_func_var); ids.isNotDefinedFct($3); $$->str=$3; }
      | ID '.' ID                          {ids.isNotDefinedVar($1, def_func_var); ids.isNotDefinedVar($3, def_func_var); $$->str=$3; }
     ;     
   

// expr : expr '+' expr                       {$$->ast = tree.buildAST($1->ast, $3->ast, ADD); char* a = (char *)malloc(10); sprintf(a, "%s+%s", $1->str, $3->str); /*printf("Expresia este: %s\n", a);*/ $$->str=a;}
//       | expr '-' expr                      {$$->ast = tree.buildAST($1->ast, $3->ast, SUB); char* b = (char *)malloc(10); sprintf(b, "%s-%s", $1->str, $3->str);/* printf("Expresia este: %s\n", b);*/ $$->str=b;}
//       | expr '*' expr                      {$$->ast = tree.buildAST($1->ast, $3->ast, MUL); char* c = (char *)malloc(10); sprintf(c, "%s*%s", $1->str, $3->str); /*printf("Expresia este: %s\n", c); */$$->str=c;}
//       | expr '/'  expr                     {$$->ast = tree.buildAST($1->ast, $3->ast, DIV); char* d = (char *)malloc(10); sprintf(d, "%s/%s", $1->str, $3->str); /*printf("Expresia este: %s\n", d); */$$->str=d;}
//       | expr '%' expr                      {$$->ast = tree.buildAST($1->ast, $3->ast, MOD); char* e = (char *)malloc(10); sprintf(e, "%s%%%s", $1->str, $3->str); /*printf("Expresia este: %s\n", e); */$$->str=e;}
//       | '(' expr ')'                       {$$ = $2;  char* m = (char *)malloc(10); sprintf(m, "%s", $2->str); $$->str = m;}
//       | expr AND expr                      {char* f = (char *)malloc(10); sprintf(f, "%s %s %s", $1->str, $2, $3->str);/* printf("Expresia este: %s\n", f);*/ $$->str=f;}
//       | expr OR expr                       {char* g = (char *)malloc(10); sprintf(g, "%s %s %s", $1->str, $2, $3->str); /*printf("Expresia este: %s\n", g); */$$->str=g;}
//       | NOT expr                           {char* h = (char *)malloc(10); sprintf(h, "%s%s", $1, $2->str); /*printf("Expresia este: %s\n", h); */$$->str=h;}
//       | expr OPR expr                      {char* i = (char *)malloc(10); sprintf(i, "%s %s %s", $1->str, $2, $3->str); /*printf("Expresia este: %s\n", i);*/ $$->str=i;}
//       | ID                                 {ids.isNotDefinedVar($1, def_func_var); $$->str=$1; }
//       | ID '[' ID ']'                      { char n[10]; strcpy(n, $1); strcat(n, "["); ids.isNotDefinedVar(n, def_func_var); ids.isNotDefinedVar($3, def_func_var); char* j = (char *)malloc(10); sprintf(j, "%s[%s]", $1, $3); $$->str=j;}
//       | ID '[' INT_VALUE ']'               { char n[10]; strcpy(n, $1); strcat(n, "["); ids.isNotDefinedVar(n, def_func_var); char* k = (char *)malloc(10); sprintf(k, "%s[%s]", $1, $3); $$->str=k;}
//       | ID '['']'                          {char n[10]; strcpy(n, $1); strcat(n, "["); ids.isNotDefinedVar(n, def_func_var); char* l = (char *)malloc(10); sprintf(l, "%s[ ]", $1); $$->str=l;}
//       | BOOL_VALUE                         { ids.addConstant($1, "bool", $1, def_func_var, false); $$->str=$1; if(strcmp($1,"true")==0) {$$->ast = tree.addNode(1);} else {$$->ast = tree.addNode(0);}}
//       | INT_VALUE                          { ids.addConstant($1, "int", $1, def_func_var, false); $$->str=$1; $$->ast = tree.addNode(atoi($1));}
//       | FLOAT_VALUE                        { ids.addConstant($1, "float", $1, def_func_var, false); $$->str=$1; $$->ast = tree.addNode(atoi($1));}
//       | STRING_VALUE                       { ids.addConstant($1, "string", $1, def_func_var, false); $$->str=$1;}
//       | CHAR_VALUE                         { ids.addConstant($1, "char", $1, def_func_var, false); $$->str=$1;}
//       | ID '(' list_param_call ')'         {ids.isNotDefinedFct($1);  $$->str=$1; int ct = ids.sFct($1); if(ct>-1) {$$->ast=tree.addNode(0);}}
//       | ID '(' ')'                         {ids.isNotDefinedFct($1); ids.isNotDefinedFct($1); $$->str=$1; }
//       | ID '.' ID '(' list_param_call ')'  {ids.isNotDefinedVar($1, def_func_var); ids.isNotDefinedFct($3); $$->str=$3; }
//       | ID '.' ID '(' ')'                  {ids.isNotDefinedVar($1, def_func_var); ids.isNotDefinedFct($3); $$->str=$3; }
//       ;     

// func_call: ID'(' list_param_call ')'  {ids.isNotDefinedFct($1);} 
//          | ID '(' ')'                 {ids.isNotDefinedFct($1);}
//          ;

list_param_call: list_param_call ',' expr {ids.verifExpr(ids.typeOf($3->str, def_func_var));}
               | expr {ids.verifExpr(ids.typeOf($1->str, def_func_var));}
               ;

main: MAIN {strcpy(def_func_var, "main");}'(' ')' '{' func_body '}'


class_section: class_section class
             | class
             ;

class: CLASS ID {ids.addClass($2); strcpy(def_func_var, "Class "); strcat(def_func_var, $2); strcpy(def_from_func, "Class "); strcat(def_from_func, $2);}'{' class_body '}' 

class_body: ACCESS_MOD ':' attribute_section
          | ACCESS_MOD ':' method_section
          | ACCESS_MOD ':' attribute_section  ACCESS_MOD ':' method_section
          ;

attribute_section: attribute_section attribute
                 | attribute
                 ;

attribute: TYPE ID ';'                               {  ids.addVar($2, $1, "", def_func_var, false) ;}
         | TYPE ID ASSIGN expr ';'                   { ids.addVar($2, $1, "", def_func_var, false); if(ids.exprOk(ids.typeOf($4->str, def_func_var))){ids.verifTypeVar($2, def_func_var, ids.typeOf($4->str, def_func_var));}}
         | CONST TYPE ID ';'                         { ids.addVar($3, $2, "", def_func_var, true);}
         | CONST TYPE ID ASSIGN expr ';'             { ids.addVar($3, $2, "", def_func_var, true);  if(ids.exprOk(ids.typeOf($5->str, def_func_var))){ids.verifTypeVar($3, def_func_var, ids.typeOf($5->str, def_func_var));}}
         | TYPE ID '[' INT_VALUE']' ';'              { char s[10]; sprintf(s, "%s[%s]", $2, $4); ids.addVar(s, $1, "", def_func_var, false);}
         | CONST TYPE ID '[' INT_VALUE']' ';'        { char s[10]; sprintf(s, "%s[%s]", $3, $5); ids.addVar(s, $2, "", def_func_var, true);}
         ;



method_section: constructor methods
              | constructor
              | methods
              ;
//constructor
constructor:CONSTRUCTOR ID '(' {strcpy(nume_fct,$2); strcpy(def_func_var, $2); ids.addFct($2, $2,"", def_from_func,false);} list_param ')' '{' func_body '}'
           |CONSTRUCTOR ID '(' {strcpy(nume_fct,$2); strcpy(def_func_var, $2); ids.addFct($2, $2,"", def_from_func,false);}')' '{' func_body '}'
           ;

methods: methods method
       | method
       ;
      

method:TYPE ID '(' {strcpy(nume_fct,$2); strcpy(def_func_var, $2); ids.addFct($2,$1,"", def_from_func,false); } list_param ')' '{' func_body '}' 
      |TYPE ID '(' {strcpy(nume_fct,$2); strcpy(def_func_var, $2); ids.addFct($2,$1,"", def_from_func,false);} ')' '{' func_body '}' 
      ;

%%

void yyerror(const char * s){
printf("error: %s at line:%d\n",s,yylineno);
}

int main(int argc, char** argv){
     yyin=fopen(argv[1],"r");
     yyparse();
    // cout << "Variables:" <<endl;
     //ids.printVars();
     ids.printVarsTable();
     ids.printFctsTable();
}
