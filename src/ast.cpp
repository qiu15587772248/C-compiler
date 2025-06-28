#include "ast.h"
#include <iomanip>

// 辅助函数：打印缩进
void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) {
        std::cerr << "  ";
    }
}

// 整数字面量节点实现
void IntegerLiteral::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void IntegerLiteral::print(int indent) const {
    printIndent(indent);
    std::cerr << "整数字面量: " << value << std::endl;
}

// 标识符节点实现
void Identifier::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void Identifier::print(int indent) const {
    printIndent(indent);
    std::cerr << "标识符: " << name << std::endl;
}

// 二元表达式节点实现
void BinaryExpression::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void BinaryExpression::print(int indent) const {
    printIndent(indent);
    std::cerr << "二元表达式: " << op << std::endl;
    printIndent(indent);
    std::cerr << "左操作数:" << std::endl;
    left->print(indent + 1);
    printIndent(indent);
    std::cerr << "右操作数:" << std::endl;
    right->print(indent + 1);
}

// 一元表达式节点实现
void UnaryExpression::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void UnaryExpression::print(int indent) const {
    printIndent(indent);
    std::cerr << "一元表达式: " << op << std::endl;
    operand->print(indent + 1);
}

// 赋值表达式节点实现
void AssignmentExpression::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void AssignmentExpression::print(int indent) const {
    printIndent(indent);
    std::cerr << "赋值表达式:" << std::endl;
    printIndent(indent);
    std::cerr << "左值:" << std::endl;
    left->print(indent + 1);
    printIndent(indent);
    std::cerr << "右值:" << std::endl;
    right->print(indent + 1);
}

// 函数调用节点实现
void FunctionCall::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void FunctionCall::print(int indent) const {
    printIndent(indent);
    std::cerr << "函数调用: " << name << std::endl;
    if (!arguments.empty()) {
        printIndent(indent);
        std::cerr << "参数列表:" << std::endl;
        for (const auto& arg : arguments) {
            arg->print(indent + 1);
        }
    }
}

// 表达式语句节点实现
void ExpressionStatement::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void ExpressionStatement::print(int indent) const {
    printIndent(indent);
    std::cerr << "表达式语句:" << std::endl;
    expression->print(indent + 1);
}

// 变量声明节点实现
void VariableDeclaration::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void VariableDeclaration::print(int indent) const {
    printIndent(indent);
    std::cerr << "变量声明: " << type;
    for (const auto& name : names) {
        std::cerr << " " << name;
    }
    for (const auto& initDecl : initDeclarators) {
        std::cerr << " " << initDecl.first;
        if (initDecl.second) {
            std::cerr << " (带初始化)";
        }
    }
    std::cerr << std::endl;
}

// 复合语句节点实现（代码块）
void CompoundStatement::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void CompoundStatement::print(int indent) const {
    printIndent(indent);
    std::cerr << "复合语句块:" << std::endl;
    for (const auto& stmt : statements) {
        stmt->print(indent + 1);
    }
}

// if语句节点实现
void IfStatement::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void IfStatement::print(int indent) const {
    printIndent(indent);
    std::cerr << "if语句:" << std::endl;
    printIndent(indent);
    std::cerr << "条件:" << std::endl;
    condition->print(indent + 1);
    printIndent(indent);
    std::cerr << "then分支:" << std::endl;
    thenStmt->print(indent + 1);
    if (elseStmt) {
        printIndent(indent);
        std::cerr << "else分支:" << std::endl;
        elseStmt->print(indent + 1);
    }
}

// while循环语句节点实现
void WhileStatement::accept(Visitor* visitor) {
    visitor->visit(this);  // 接受访问者，调用访问者的visit方法
}

void WhileStatement::print(int indent) const {
    printIndent(indent);
    std::cerr << "while循环:" << std::endl;
    printIndent(indent);
    std::cerr << "循环条件:" << std::endl;
    condition->print(indent + 1);
    printIndent(indent);
    std::cerr << "循环体:" << std::endl;
    body->print(indent + 1);
}

// ForStatement 实现
void ForStatement::accept(Visitor* visitor) {
    visitor->visit(this);
}

void ForStatement::print(int indent) const {
    printIndent(indent);
    std::cerr << "ForStatement:" << std::endl;
    if (init) {
        printIndent(indent);
        std::cerr << "Init:" << std::endl;
        init->print(indent + 1);
    }
    if (condition) {
        printIndent(indent);
        std::cerr << "Condition:" << std::endl;
        condition->print(indent + 1);
    }
    if (update) {
        printIndent(indent);
        std::cerr << "Update:" << std::endl;
        update->print(indent + 1);
    }
    printIndent(indent);
    std::cerr << "Body:" << std::endl;
    body->print(indent + 1);
}

// ReturnStatement 实现
void ReturnStatement::accept(Visitor* visitor) {
    visitor->visit(this);
}

void ReturnStatement::print(int indent) const {
    printIndent(indent);
    std::cerr << "ReturnStatement:" << std::endl;
    if (value) {
        value->print(indent + 1);
    }
}

// FunctionDefinition 实现
void FunctionDefinition::accept(Visitor* visitor) {
    visitor->visit(this);
}

void FunctionDefinition::print(int indent) const {
    printIndent(indent);
    std::cerr << "FunctionDefinition: " << returnType << " " << name << "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) std::cerr << ", ";
        std::cerr << parameters[i].first << " " << parameters[i].second;
    }
    std::cerr << ")" << std::endl;
    if (body) {
        body->print(indent + 1);
    }
}

// Program 实现
void Program::accept(Visitor* visitor) {
    visitor->visit(this);
}

void Program::print(int indent) const {
    printIndent(indent);
    std::cerr << "Program:" << std::endl;
    for (const auto& decl : declarations) {
        decl->print(indent + 1);
    }
}

// =============== printWithSemantics 实现 ===============

// 辅助函数：打印语义信息
void printSemanticInfo(const SemanticInfo& info, int indent) {
    if (!info.type.empty() || info.hasSemanticError || !info.symbolKind.empty()) {
        printIndent(indent);
        std::cerr << "[语义信息: ";
        if (!info.type.empty()) {
            std::cerr << "类型=" << info.type << " ";
        }
        if (!info.symbolKind.empty()) {
            std::cerr << "种类=" << info.symbolKind << " ";
        }
        if (info.scopeLevel > 0) {
            std::cerr << "作用域=" << info.scopeLevel << " ";
        }
        if (info.isInitialized) {
            std::cerr << "已初始化 ";
        }
        if (info.hasSemanticError) {
            std::cerr << "错误: " << info.errorMessage << " ";
        }
        std::cerr << "]" << std::endl;
    }
}

void IntegerLiteral::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "IntegerLiteral: " << value << std::endl;
    printSemanticInfo(semanticInfo, indent);
}

void Identifier::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "Identifier: " << name << std::endl;
    printSemanticInfo(semanticInfo, indent);
}

void BinaryExpression::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "BinaryExpression: " << op << std::endl;
    printSemanticInfo(semanticInfo, indent);
    printIndent(indent);
    std::cerr << "Left:" << std::endl;
    left->printWithSemantics(indent + 1);
    printIndent(indent);
    std::cerr << "Right:" << std::endl;
    right->printWithSemantics(indent + 1);
}

void UnaryExpression::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "UnaryExpression: " << op << std::endl;
    printSemanticInfo(semanticInfo, indent);
    operand->printWithSemantics(indent + 1);
}

void AssignmentExpression::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "AssignmentExpression:" << std::endl;
    printSemanticInfo(semanticInfo, indent);
    printIndent(indent);
    std::cerr << "Left:" << std::endl;
    left->printWithSemantics(indent + 1);
    printIndent(indent);
    std::cerr << "Right:" << std::endl;
    right->printWithSemantics(indent + 1);
}

void FunctionCall::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "FunctionCall: " << name << std::endl;
    printSemanticInfo(semanticInfo, indent);
    if (!arguments.empty()) {
        printIndent(indent);
        std::cerr << "Arguments:" << std::endl;
        for (const auto& arg : arguments) {
            arg->printWithSemantics(indent + 1);
        }
    }
}

void ExpressionStatement::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "ExpressionStatement:" << std::endl;
    printSemanticInfo(semanticInfo, indent);
    expression->printWithSemantics(indent + 1);
}

void VariableDeclaration::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "VariableDeclaration: " << type;
    for (const auto& name : names) {
        std::cerr << " " << name;
    }
    for (const auto& initDecl : initDeclarators) {
        std::cerr << " " << initDecl.first;
        if (initDecl.second) {
            std::cerr << " (with initializer)";
        }
    }
    std::cerr << std::endl;
    printSemanticInfo(semanticInfo, indent);
}

void CompoundStatement::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "CompoundStatement:" << std::endl;
    printSemanticInfo(semanticInfo, indent);
    for (const auto& stmt : statements) {
        stmt->printWithSemantics(indent + 1);
    }
}

void IfStatement::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "IfStatement:" << std::endl;
    printSemanticInfo(semanticInfo, indent);
    printIndent(indent);
    std::cerr << "Condition:" << std::endl;
    condition->printWithSemantics(indent + 1);
    printIndent(indent);
    std::cerr << "Then:" << std::endl;
    thenStmt->printWithSemantics(indent + 1);
    if (elseStmt) {
        printIndent(indent);
        std::cerr << "Else:" << std::endl;
        elseStmt->printWithSemantics(indent + 1);
    }
}

void WhileStatement::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "WhileStatement:" << std::endl;
    printSemanticInfo(semanticInfo, indent);
    printIndent(indent);
    std::cerr << "Condition:" << std::endl;
    condition->printWithSemantics(indent + 1);
    printIndent(indent);
    std::cerr << "Body:" << std::endl;
    body->printWithSemantics(indent + 1);
}

void ForStatement::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "ForStatement:" << std::endl;
    printSemanticInfo(semanticInfo, indent);
    if (init) {
        printIndent(indent);
        std::cerr << "Init:" << std::endl;
        init->printWithSemantics(indent + 1);
    }
    if (condition) {
        printIndent(indent);
        std::cerr << "Condition:" << std::endl;
        condition->printWithSemantics(indent + 1);
    }
    if (update) {
        printIndent(indent);
        std::cerr << "Update:" << std::endl;
        update->printWithSemantics(indent + 1);
    }
    printIndent(indent);
    std::cerr << "Body:" << std::endl;
    body->printWithSemantics(indent + 1);
}

void ReturnStatement::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "ReturnStatement:" << std::endl;
    printSemanticInfo(semanticInfo, indent);
    if (value) {
        value->printWithSemantics(indent + 1);
    }
}

void FunctionDefinition::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "FunctionDefinition: " << returnType << " " << name << "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) std::cerr << ", ";
        std::cerr << parameters[i].first << " " << parameters[i].second;
    }
    std::cerr << ")" << std::endl;
    printSemanticInfo(semanticInfo, indent);
    if (body) {
        body->printWithSemantics(indent + 1);
    }
}

void Program::printWithSemantics(int indent) const {
    printIndent(indent);
    std::cerr << "Program:" << std::endl;
    printSemanticInfo(semanticInfo, indent);
    for (const auto& decl : declarations) {
        decl->printWithSemantics(indent + 1);
    }
} 
 