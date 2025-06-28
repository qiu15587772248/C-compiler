%{
// 定义特性测试宏
#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _GNU_SOURCE

// 避免包含unistd.h（Flex生成的代码默认包含该Unix头文件，但Windows系统没有这个文件）
#define YY_NO_UNISTD_H

#include "ast.h"
#include "parser.tab.hpp"
#include <string>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

// 手动声明fileno
#ifndef fileno
extern "C" int fileno(FILE *stream);
#endif

extern int yylineno;

// strdup函数的简单实现
#ifndef strdup
static char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* result = (char*)malloc(len);
    if (result) {
        memcpy(result, s, len);
    }
    return result;
}
#endif
%}

%option noyywrap
%option yylineno
%option never-interactive

/* 定义正则表达式 */
DIGIT       [0-9]
LETTER      [a-zA-Z_]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
INTEGER     {DIGIT}+
WHITESPACE  [ \t\r]
NEWLINE     \n

%%

{WHITESPACE}+   { /* 忽略空白字符 */ }
{NEWLINE}       { /* 忽略换行，但yylineno会自动增加 */ }

"/*"([^*]|\*+[^*/])*\*+"/"   { /* 忽略C风格注释 */ }
"//".*                       { /* 忽略C++风格注释 */ }

"int"           { return INT; }
"char"          { return CHAR; }
"float"         { return FLOAT; }
"double"        { return DOUBLE; }
"void"          { return VOID; }

"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"for"           { return FOR; }
"return"        { return RETURN; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }

"=="            { return EQ; }
"!="            { return NE; }
"<="            { return LE; }
">="            { return GE; }
"&&"            { return AND; }
"||"            { return OR; }
"++"            { return INC; }
"--"            { return DEC; }

"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"%"             { return '%'; }
"="             { return '='; }
"<"             { return '<'; }
">"             { return '>'; }
"!"             { return '!'; }

"("             { return '('; }
")"             { return ')'; }
"{"             { return '{'; }
"}"             { return '}'; }
"["             { return '['; }
"]"             { return ']'; }
";"             { return ';'; }
","             { return ','; }

{INTEGER}       { 
                    yylval.intval = atoi(yytext); 
                    return INTEGER_LITERAL; 
                }

{IDENTIFIER}    { 
                    yylval.strval = strdup(yytext); 
                    return IDENTIFIER; 
                }

.               { 
                    if (yytext[0] == '"') {
                        printf("[词法错误] 行 %d: 不支持的字符串字面量 '%c'，当前编译器不支持字符串类型\n", yylineno, yytext[0]);
                    } else if (yytext[0] == '\'') {
                        printf("[词法错误] 行 %d: 不支持的字符字面量 '%c'，当前编译器不支持字符字面量\n", yylineno, yytext[0]);
                    } else if (yytext[0] >= 32 && yytext[0] <= 126) {
                        printf("[词法错误] 行 %d: 无效字符 '%c' (ASCII %d)，不在词法规则范围内\n", yylineno, yytext[0], yytext[0]);
                    } else {
                        printf("[词法错误] 行 %d: 无效字符 (ASCII %d)，不可打印字符\n", yylineno, yytext[0]);
                    }
                    return ERROR_TOKEN;
                }

%%

/* 可以添加辅助函数 */
void init_lexer() {
    yylineno = 1;
} 