%start ROOT

%{
#include "def.h"
%}

%union {
    char *str;
    NUM_TYPE num;
    ID_TYPE id;
    struct ast_node* astnode;
} 

%token EQ
%token NE
%token LT
%token LE
%token GT
%token GE
%token PLUS
%token MINUS
%token NEG
%token MULT
%token DIVIDE
%token RPAREN
%token LPAREN
%token ASSIGN
%token SEMICOLON
%token IF
%token THEN
%token ELSE
%token FI
%token WHILE
%token DO
%token OD
%token PRINT
%token <num> NUMBER
%token <id> NAME

%type <astnode> statement
%type <astnode> expression
%type <astnode> ROOT
%type <astnode> stmtseq
%type <astnode> designator
%type <astnode> expr2
%type <astnode> expr3
%type <astnode> expr4


%%

ROOT:
stmtseq { execute($1); } 
;

statement:
designator ASSIGN expression SEMICOLON { $$ = assignment($1, $3); } 
| PRINT expression SEMICOLON { $$ = print($2); } 
| IF expression THEN stmtseq ELSE stmtseq FI
{ $$ = ifstmt($2, $4, $6); }
| IF expression THEN stmtseq FI
{ $$ = ifstmt($2, $4, empty()); }
| WHILE expression DO stmtseq OD { $$ = whilestmt($2, $4); }   
;

stmtseq:
stmtseq statement { $$ = seq($1, $2); }
| statement { $$ = seq(NULL, $1); }
;

expression:
expr2 { $$ = $1; } 
| expr2 EQ expr2 { $$ = eq($1, $3); }
| expr2 NE expr2 { $$ = ne($1, $3); }
| expr2 LT expr2 { $$ = lt($1, $3); }
| expr2 LE expr2 { $$ = le($1, $3); }
| expr2 GT expr2 { $$ = gt($1, $3); }
| expr2 GE expr2 { $$ = ge($1, $3); }
;

expr2:
expr3 { $$ == $1; }
| expr2 PLUS expr3 { $$ = plus($1, $3); }
| expr2 MINUS expr3 { $$ = minus($1, $3); }
;

expr3:
expr4 { $$ = $1; }
| expr3 MULT expr4 { $$ = mult($1, $3); }
| expr3 DIVIDE expr4 { $$ = divide ($1, $3); }
;

expr4:
PLUS expr4 { $$ = $2; }
| MINUS expr4 { $$ = neg($2); }
| LPAREN expression RPAREN { $$ = $2; }
| NUMBER { $$ = number($1); }
| designator { $$ = $1; }
;

designator:
NAME { $$ = name($1); }
;

