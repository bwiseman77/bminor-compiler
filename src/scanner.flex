%{
#include "token.h"
%}
DIGIT [0-9]+
LETTER [a-zA-Z]
MLCOM \/\*[^\*]*\*+([^\*\/][^\*]*\**)*[*]*\/
IDENT (_|{LETTER})(_|{LETTER}|[0-9])* 
CHAR \'([^\\\']|(\\.))\' 
STRING \"(([^\"\0\n]*)|(\\.))*\"  
%%
[[:space:]]+            /* skip */
\/\/[^\n]*\n            /* { return TOKEN_SINGLE_LINE; }  comments        */
{MLCOM}                 /* { return TOKEN_MULTI_LINE; }   skip for parser */
array                   { return TOKEN_ARRAY; }         /* keywords */
auto                    { return TOKEN_AUTO; }
boolean                 { return TOKEN_BOOLEAN; }
char                    { return TOKEN_CHAR; }
else                    { return TOKEN_ELSE; }
for                     { return TOKEN_FOR; }
function                { return TOKEN_FUNCTION; }
if                      { return TOKEN_IF; }
integer                 { return TOKEN_INTEGER; }
print                   { return TOKEN_PRINT; }
return                  { return TOKEN_RETURN; }
string                  { return TOKEN_STRING; }
true                    { return TOKEN_TRUE; }
false                   { return TOKEN_FALSE; } 
void                    { return TOKEN_VOID; }
while                   { return TOKEN_WHILE; }
\+                      { return TOKEN_ADD; }           /* expressions */
\-                      { return TOKEN_SUB; }
=                       { return TOKEN_ASSIGN; }
\+\+                    { return TOKEN_INCREMENT; }
\-\-                    { return TOKEN_DECREMENT; }
!                       { return TOKEN_NOT; }
\^                      { return TOKEN_EXPONENT; }
\*                      { return TOKEN_MULTIPLICATION; }
\/                      { return TOKEN_DIVISON; }
%                       { return TOKEN_MODULO; }
\<                      { return TOKEN_LT; }
\<=                     { return TOKEN_LTE; }
\>                      { return TOKEN_GT; }
\>=                     { return TOKEN_GTE; }
==                      { return TOKEN_EQ; }
!=                      { return TOKEN_NOT_EQ; }
&&                      { return TOKEN_AND; }
\|\|                    { return TOKEN_OR; }
\?                      { return TOKEN_TERNARY_PT_1; }
{IDENT}                 { return TOKEN_IDENT; }         /* literals */
{CHAR}                  { return TOKEN_CHAR_LITERAL; }
{STRING}                { return TOKEN_STRING_LITERAL; }
{DIGIT}+                { return TOKEN_INTEGER_LITERAL; }
:                       { return TOKEN_COLON; }         /* misc */
;                       { return TOKEN_SEMICOLON; }
,                       { return TOKEN_COMMA; }
\{                      { return TOKEN_LEFT_BRACE; }
\[                      { return TOKEN_LEFT_BRACKET; }
\(                      { return TOKEN_LEFT_PARENTH; }
\}                      { return TOKEN_RIGHT_BRACE; }
\]                      { return TOKEN_RIGHT_BRACKET; }
\)                      { return TOKEN_RIGHT_PARENTH; }
.                       { return TOKEN_ERROR; } 
%%
int yywrap() { return 1; }
