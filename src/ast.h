#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// 前向声明
class Visitor;

// 语义信息结构
struct SemanticInfo {
    std::string type;           // 类型信息
    bool isInitialized;         // 是否已初始化
    int scopeLevel;             // 作用域层级
    std::string symbolKind;     // 符号种类：variable, function, parameter
    bool hasSemanticError;      // 是否有语义错误
    std::string errorMessage;   // 错误信息
    
    SemanticInfo() : isInitialized(false), scopeLevel(0), hasSemanticError(false) {}
};

// AST节点基类
class ASTNode {
public:
    SemanticInfo semanticInfo;  // 语义信息
    int lineNumber;             // 行号信息
    
    ASTNode() : lineNumber(0) {}
    virtual ~ASTNode() = default;
    virtual void accept(Visitor* visitor) = 0;
    virtual void print(int indent = 0) const = 0;
    virtual void printWithSemantics(int indent = 0) const = 0;  // 打印带语义信息的树
};

// 表达式基类
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

// 语句基类
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

// 整数字面量
class IntegerLiteral : public Expression {
public:
    int value;
    
    IntegerLiteral(int val) : value(val) {}
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 标识符
class Identifier : public Expression {
public:
    std::string name;
    
    Identifier(const std::string& n) : name(n) {}
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 二元运算表达式
class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::string op;
    std::unique_ptr<Expression> right;
    
    BinaryExpression(std::unique_ptr<Expression> l, const std::string& o, std::unique_ptr<Expression> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
    
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 一元运算表达式
class UnaryExpression : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> operand;
    
    UnaryExpression(const std::string& o, std::unique_ptr<Expression> expr)
        : op(o), operand(std::move(expr)) {}
    
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 赋值表达式
class AssignmentExpression : public Expression {
public:
    std::unique_ptr<Identifier> left;
    std::unique_ptr<Expression> right;
    
    AssignmentExpression(std::unique_ptr<Identifier> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}
    
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 函数调用表达式
class FunctionCall : public Expression {
public:
    std::string name;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    FunctionCall(const std::string& n) : name(n) {}
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 表达式语句
class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    
    ExpressionStatement(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 变量声明
class VariableDeclaration : public Statement {
public:
    std::string type;
    std::vector<std::string> names;
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>> initDeclarators;
    
    VariableDeclaration(const std::string& t) : type(t) {}
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 复合语句（代码块）
class CompoundStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// If语句
class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenStmt;
    std::unique_ptr<Statement> elseStmt; // 可选
    
    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> then_stmt)
        : condition(std::move(cond)), thenStmt(std::move(then_stmt)), elseStmt(nullptr) {}
    
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// While语句
class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    
    WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> b)
        : condition(std::move(cond)), body(std::move(b)) {}
    
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// For语句
class ForStatement : public Statement {
public:
    std::unique_ptr<Statement> init;      // 初始化语句
    std::unique_ptr<Expression> condition; // 条件表达式
    std::unique_ptr<Expression> update;    // 更新表达式
    std::unique_ptr<Statement> body;       // 循环体
    
    ForStatement(std::unique_ptr<Statement> i, std::unique_ptr<Expression> c, 
                std::unique_ptr<Expression> u, std::unique_ptr<Statement> b)
        : init(std::move(i)), condition(std::move(c)), update(std::move(u)), body(std::move(b)) {}
    
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// Return语句
class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value; 
    
    ReturnStatement(std::unique_ptr<Expression> val = nullptr) : value(std::move(val)) {}
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 函数定义
class FunctionDefinition : public ASTNode {
public:
    std::string returnType;
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters; // (type, name)
    std::unique_ptr<CompoundStatement> body;
    
    FunctionDefinition(const std::string& ret_type, const std::string& n)
        : returnType(ret_type), name(n) {}
    
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 程序根节点
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> declarations; // 函数定义和全局变量声明
    
    void accept(Visitor* visitor) override;
    void print(int indent = 0) const override;
    void printWithSemantics(int indent = 0) const override;
};

// 访问者模式接口
class Visitor {
public:
    virtual ~Visitor() = default;
    
    virtual void visit(IntegerLiteral* node) = 0;
    virtual void visit(Identifier* node) = 0;
    virtual void visit(BinaryExpression* node) = 0;
    virtual void visit(UnaryExpression* node) = 0;
    virtual void visit(AssignmentExpression* node) = 0;
    virtual void visit(FunctionCall* node) = 0;
    virtual void visit(ExpressionStatement* node) = 0;
    virtual void visit(VariableDeclaration* node) = 0;
    virtual void visit(CompoundStatement* node) = 0;
    virtual void visit(IfStatement* node) = 0;
    virtual void visit(WhileStatement* node) = 0;
    virtual void visit(ForStatement* node) = 0;
    virtual void visit(ReturnStatement* node) = 0;
    virtual void visit(FunctionDefinition* node) = 0;
    virtual void visit(Program* node) = 0;
};

#endif 