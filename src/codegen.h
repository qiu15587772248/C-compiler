#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

// x86汇编代码生成器
class CodeGenerator : public Visitor {
private:
    std::ostream& output;
    std::unordered_map<std::string, int> symbolTable; // 变量名到栈偏移的映射
    int stackOffset;        // 当前栈偏移
    int labelCounter;       // 标签计数器
    std::string currentFunction; // 当前函数名
    
    // 生成唯一标签
    std::string generateLabel(const std::string& prefix = "L");
    
    // 分配栈空间给变量
    void allocateVariable(const std::string& name);
    
    // 获取变量的栈地址
    std::string getVariableAddress(const std::string& name);
    
    // 生成函数前导码
    void generateFunctionPrologue(const std::string& funcName);
    
    // 生成函数后导码
    void generateFunctionEpilogue();

public:
    CodeGenerator(std::ostream& out);
    
    // 访问者模式实现
    void visit(IntegerLiteral* node) override;
    void visit(Identifier* node) override;
    void visit(BinaryExpression* node) override;
    void visit(UnaryExpression* node) override;
    void visit(AssignmentExpression* node) override;
    void visit(FunctionCall* node) override;
    void visit(ExpressionStatement* node) override;
    void visit(VariableDeclaration* node) override;
    void visit(CompoundStatement* node) override;
    void visit(IfStatement* node) override;
    void visit(WhileStatement* node) override;
    void visit(ForStatement* node) override;
    void visit(ReturnStatement* node) override;
    void visit(FunctionDefinition* node) override;
    void visit(Program* node) override;
    
    // 生成汇编代码
    void generateAssembly(Program* program);
};

#endif // CODEGEN_H 