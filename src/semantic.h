#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

// 符号信息结构
struct SymbolInfo {
    std::string name;
    std::string type;
    std::string kind; // "variable", "function", "parameter"
    int scopeLevel;
    bool isInitialized;
    
    SymbolInfo(const std::string& n, const std::string& t, const std::string& k, int level = 0)
        : name(n), type(t), kind(k), scopeLevel(level), isInitialized(false) {}
};

// 符号表类
class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, std::unique_ptr<SymbolInfo>>> scopes;
    int currentScope;

public:
    SymbolTable();
    ~SymbolTable() = default;
    
    void enterScope();
    void exitScope();
    bool declare(const std::string& name, const std::string& type, const std::string& kind);
    SymbolInfo* lookup(const std::string& name);
    SymbolInfo* lookupInCurrentScope(const std::string& name);
    void print() const;
    int getCurrentScopeLevel() const { return currentScope; }
};

// 类型信息结构
struct TypeInfo {
    std::string baseType;
    bool isValid;
    
    TypeInfo(const std::string& type = "void", bool valid = true) 
        : baseType(type), isValid(valid) {}
        
    bool isNumeric() const {
        return baseType == "int" || baseType == "char" || baseType == "float" || baseType == "double";
    }
    
    bool isInteger() const {
        return baseType == "int" || baseType == "char";
    }
    
    bool canAssignTo(const TypeInfo& target) const {
        if (!isValid || !target.isValid) return false;
        if (baseType == target.baseType) return true;
        if (isNumeric() && target.isNumeric()) return true; // 允许数值类型间的隐式转换
        return false;
    }
};

// 语义错误类
class SemanticError {
public:
    std::string message;
    std::string errorType;  // 错误类型
    int line;
    int column;             // 列号
    std::string context;    // 错误上下文
    
    SemanticError(const std::string& msg, const std::string& type = "语义错误", int l = 0, int c = 0, const std::string& ctx = "") 
        : message(msg), errorType(type), line(l), column(c), context(ctx) {}
};

// 语义分析器类
class SemanticAnalyzer : public Visitor {
private:
    SymbolTable symbolTable;
    std::vector<SemanticError> errors;
    std::vector<std::string> warnings;
    TypeInfo currentExpressionType;
    std::string currentFunctionReturnType;
    bool hasReturnStatement;
    int currentLine;        // 当前行号
    std::string currentContext; // 当前上下文

public:
    SemanticAnalyzer() : hasReturnStatement(false), currentLine(0) {}
    ~SemanticAnalyzer() = default;
    
    // 主要分析函数
    bool analyze(Program* program, bool silent = false);
    void printResults() const;
    void printSemanticTree(Program* program) const;
    bool hasErrors() const { return !errors.empty(); }
    
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

private:
    // 辅助函数
    void addError(const std::string& message, const std::string& errorType = "语义错误", const std::string& context = "");
    void addWarning(const std::string& message);
    void setCurrentLine(int line) { currentLine = line; }
    void setCurrentContext(const std::string& context) { currentContext = context; }
    TypeInfo getExpressionType(Expression* expr);
    bool isValidBinaryOperation(const std::string& op, const TypeInfo& left, const TypeInfo& right);
    bool isValidUnaryOperation(const std::string& op, const TypeInfo& operand);
    std::string getResultType(const std::string& op, const TypeInfo& left, const TypeInfo& right);
};

#endif 