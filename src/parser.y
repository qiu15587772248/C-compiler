%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yylineno;
extern FILE* yyin;

void yyerror(const char* msg);

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

// 全局变量存储解析结果
Program* program_root = nullptr;

// 辅助函数：为AST节点设置行号
template<typename T>
T* setLineNumber(T* node, int line) {
    if (node) {
        node->lineNumber = line;
    }
    return node;
}
%}

%union {
    int intval;
    char* strval;
    ASTNode* node;
    Expression* expr;
    Statement* stmt;
    Program* program;
    FunctionDefinition* func_def;
    VariableDeclaration* var_decl;
    CompoundStatement* compound_stmt;
    std::vector<std::unique_ptr<Statement>>* stmt_list;
    std::vector<std::string>* str_list;
    std::vector<std::pair<std::string, std::string>>* param_list;
    std::vector<std::unique_ptr<Expression>>* expr_list;
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>>* init_decl_list;
    std::pair<std::string, std::unique_ptr<Expression>>* init_decl;
}

/* 终结符定义 */
%token <intval> INTEGER_LITERAL
%token <strval> IDENTIFIER
%token INT CHAR FLOAT DOUBLE VOID
%token IF ELSE WHILE FOR RETURN BREAK CONTINUE
%token EQ NE LE GE AND OR INC DEC
%token ERROR_TOKEN

/* 非终结符类型定义 */
%type <program> program
%type <node> declaration
%type <func_def> function_definition
%type <var_decl> variable_declaration
%type <compound_stmt> compound_statement
%type <stmt> statement
%type <stmt_list> statement_list
%type <expr> expression
%type <expr> assignment_expression
%type <expr> logical_or_expression
%type <expr> logical_and_expression
%type <expr> equality_expression
%type <expr> relational_expression
%type <expr> additive_expression
%type <expr> multiplicative_expression
%type <expr> unary_expression
%type <expr> primary_expression
%type <expr> postfix_expression
%type <strval> type_specifier
%type <str_list> identifier_list
%type <param_list> parameter_list
%type <expr_list> argument_list
%type <init_decl_list> init_declarator_list
%type <init_decl> init_declarator

/* 运算符优先级和结合性 */
%right '='
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right UMINUS '!' INC DEC
%left '(' ')' '[' ']'

%%

program:
    /* empty */
    {
        $$ = new Program();
        program_root = $$;
    }
    | program declaration
    {
        $$ = $1;
        $$->declarations.push_back(std::unique_ptr<ASTNode>($2));
    }
    ;

declaration:
    function_definition
    {
        $$ = $1;
    }
    | variable_declaration ';'
    {
        $$ = $1;
    }
    ;

function_definition:
    type_specifier IDENTIFIER '(' ')' compound_statement
    {
        $$ = setLineNumber(new FunctionDefinition($1, $2), yylineno);
        $$->body = std::unique_ptr<CompoundStatement>($5);
        free($1);
        free($2);
    }
    | type_specifier IDENTIFIER '(' parameter_list ')' compound_statement
    {
        $$ = setLineNumber(new FunctionDefinition($1, $2), yylineno);
        $$->parameters = *$4;
        $$->body = std::unique_ptr<CompoundStatement>($6);
        free($1);
        free($2);
        delete $4;
    }
    ;

parameter_list:
    type_specifier IDENTIFIER
    {
        $$ = new std::vector<std::pair<std::string, std::string>>();
        $$->push_back(std::make_pair($1, $2));
        free($1);
        free($2);
    }
    | parameter_list ',' type_specifier IDENTIFIER
    {
        $$ = $1;
        $$->push_back(std::make_pair($3, $4));
        free($3);
        free($4);
    }
    ;

variable_declaration:
    type_specifier identifier_list
    {
        $$ = setLineNumber(new VariableDeclaration($1), yylineno);
        $$->names = *$2;
        free($1);
        delete $2;
    }
    | type_specifier init_declarator_list
    {
        $$ = setLineNumber(new VariableDeclaration($1), yylineno);
        $$->initDeclarators = std::move(*$2);
        free($1);
        delete $2;
    }
    ;

identifier_list:
    IDENTIFIER
    {
        $$ = new std::vector<std::string>();
        $$->push_back($1);
        free($1);
    }
    | identifier_list ',' IDENTIFIER
    {
        $$ = $1;
        $$->push_back($3);
        free($3);
    }
    ;

init_declarator_list:
    init_declarator
    {
        $$ = new std::vector<std::pair<std::string, std::unique_ptr<Expression>>>();
        $$->push_back(std::move(*$1));
        delete $1;
    }
    | init_declarator_list ',' init_declarator
    {
        $$ = $1;
        $$->push_back(std::move(*$3));
        delete $3;
    }
    ;

init_declarator:
    IDENTIFIER
    {
        $$ = new std::pair<std::string, std::unique_ptr<Expression>>($1, nullptr);
        free($1);
    }
    | IDENTIFIER '=' assignment_expression
    {
        $$ = new std::pair<std::string, std::unique_ptr<Expression>>($1, std::unique_ptr<Expression>($3));
        free($1);
    }
    ;

type_specifier:
    INT     { $$ = strdup("int"); }
    | CHAR  { $$ = strdup("char"); }
    | FLOAT { $$ = strdup("float"); }
    | DOUBLE { $$ = strdup("double"); }
    | VOID  { $$ = strdup("void"); }
    ;

compound_statement:
    '{' '}'
    {
        $$ = new CompoundStatement();
    }
    | '{' statement_list '}'
    {
        $$ = new CompoundStatement();
        $$->statements = std::move(*$2);
        delete $2;
    }
    ;

statement_list:
    statement
    {
        $$ = new std::vector<std::unique_ptr<Statement>>();
        $$->push_back(std::unique_ptr<Statement>($1));
    }
    | statement_list statement
    {
        $$ = $1;
        $$->push_back(std::unique_ptr<Statement>($2));
    }
    ;

statement:
    expression ';'
    {
        $$ = new ExpressionStatement(std::unique_ptr<Expression>($1));
    }
    | compound_statement
    {
        $$ = $1;
    }
    | variable_declaration ';'
    {
        $$ = $1;
    }
    | IF '(' expression ')' statement
    {
        $$ = new IfStatement(std::unique_ptr<Expression>($3), std::unique_ptr<Statement>($5));
    }
    | IF '(' expression ')' statement ELSE statement
    {
        auto if_stmt = new IfStatement(std::unique_ptr<Expression>($3), std::unique_ptr<Statement>($5));
        if_stmt->elseStmt = std::unique_ptr<Statement>($7);
        $$ = if_stmt;
    }
    | WHILE '(' expression ')' statement
    {
        $$ = new WhileStatement(std::unique_ptr<Expression>($3), std::unique_ptr<Statement>($5));
    }
    | FOR '(' statement expression ';' expression ')' statement
    {
        $$ = new ForStatement(std::unique_ptr<Statement>($3), std::unique_ptr<Expression>($4), 
                             std::unique_ptr<Expression>($6), std::unique_ptr<Statement>($8));
    }
    | RETURN ';'
    {
        $$ = new ReturnStatement();
    }
    | RETURN expression ';'
    {
        $$ = new ReturnStatement(std::unique_ptr<Expression>($2));
    }
    ;

expression:
    assignment_expression
    {
        $$ = $1;
    }
    ;

assignment_expression:
    logical_or_expression
    {
        $$ = $1;
    }
    | IDENTIFIER '=' assignment_expression
    {
        $$ = setLineNumber(new AssignmentExpression(
            std::unique_ptr<Identifier>(setLineNumber(new Identifier($1), yylineno)),
            std::unique_ptr<Expression>($3)
        ), yylineno);
        free($1);
    }
    ;

logical_or_expression:
    logical_and_expression
    {
        $$ = $1;
    }
    | logical_or_expression OR logical_and_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "||",
            std::unique_ptr<Expression>($3)
        );
    }
    ;

logical_and_expression:
    equality_expression
    {
        $$ = $1;
    }
    | logical_and_expression AND equality_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "&&",
            std::unique_ptr<Expression>($3)
        );
    }
    ;

equality_expression:
    relational_expression
    {
        $$ = $1;
    }
    | equality_expression EQ relational_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "==",
            std::unique_ptr<Expression>($3)
        );
    }
    | equality_expression NE relational_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "!=",
            std::unique_ptr<Expression>($3)
        );
    }
    ;

relational_expression:
    additive_expression
    {
        $$ = $1;
    }
    | relational_expression '<' additive_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "<",
            std::unique_ptr<Expression>($3)
        );
    }
    | relational_expression '>' additive_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            ">",
            std::unique_ptr<Expression>($3)
        );
    }
    | relational_expression LE additive_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "<=",
            std::unique_ptr<Expression>($3)
        );
    }
    | relational_expression GE additive_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            ">=",
            std::unique_ptr<Expression>($3)
        );
    }
    ;

additive_expression:
    multiplicative_expression
    {
        $$ = $1;
    }
    | additive_expression '+' multiplicative_expression
    {
        $$ = setLineNumber(new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "+",
            std::unique_ptr<Expression>($3)
        ), yylineno);
    }
    | additive_expression '-' multiplicative_expression
    {
        $$ = setLineNumber(new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "-",
            std::unique_ptr<Expression>($3)
        ), yylineno);
    }
    ;

multiplicative_expression:
    unary_expression
    {
        $$ = $1;
    }
    | multiplicative_expression '*' unary_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "*",
            std::unique_ptr<Expression>($3)
        );
    }
    | multiplicative_expression '/' unary_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "/",
            std::unique_ptr<Expression>($3)
        );
    }
    | multiplicative_expression '%' unary_expression
    {
        $$ = new BinaryExpression(
            std::unique_ptr<Expression>($1),
            "%",
            std::unique_ptr<Expression>($3)
        );
    }
    ;

unary_expression:
    postfix_expression
    {
        $$ = $1;
    }
    | '-' unary_expression %prec UMINUS
    {
        $$ = new UnaryExpression("-", std::unique_ptr<Expression>($2));
    }
    | '!' unary_expression
    {
        $$ = new UnaryExpression("!", std::unique_ptr<Expression>($2));
    }
    ;

postfix_expression:
    primary_expression
    {
        $$ = $1;
    }
    | IDENTIFIER '(' ')'
    {
        $$ = setLineNumber(new FunctionCall($1), yylineno);
        free($1);
    }
    | IDENTIFIER '(' argument_list ')'
    {
        auto func_call = setLineNumber(new FunctionCall($1), yylineno);
        func_call->arguments = std::move(*$3);
        $$ = func_call;
        free($1);
        delete $3;
    }
    ;

argument_list:
    expression
    {
        $$ = new std::vector<std::unique_ptr<Expression>>();
        $$->push_back(std::unique_ptr<Expression>($1));
    }
    | argument_list ',' expression
    {
        $$ = $1;
        $$->push_back(std::unique_ptr<Expression>($3));
    }
    ;

primary_expression:
    IDENTIFIER
    {
        $$ = setLineNumber(new Identifier($1), yylineno);
        free($1);
    }
    | INTEGER_LITERAL
    {
        $$ = setLineNumber(new IntegerLiteral($1), yylineno);
    }
    | '(' expression ')'
    {
        $$ = $2;
    }
    ;

%%

void yyerror(const char* msg) {
    if (strstr(msg, "syntax error") != NULL) {
        fprintf(stderr, "[语法错误] 行 %d: 语法结构不正确，可能的原因包括：\n", yylineno);
        fprintf(stderr, "  - 缺少分号、括号或大括号\n");
        fprintf(stderr, "  - 表达式语法错误\n");
        fprintf(stderr, "  - 函数定义或变量声明格式错误\n");
        fprintf(stderr, "  - 使用了不支持的语法特性\n");
    } else {
        fprintf(stderr, "[语法错误] 行 %d: %s\n", yylineno, msg);
    }
} 