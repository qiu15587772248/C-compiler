#include "codegen.h"
#include <iostream>
#include <sstream>

CodeGenerator::CodeGenerator(std::ostream& out) 
    : output(out), stackOffset(0), labelCounter(0) {
}

std::string CodeGenerator::generateLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter++);
}

void CodeGenerator::allocateVariable(const std::string& name) {
    stackOffset += 8; // 64位环境下使用8字节对齐
    symbolTable[name] = stackOffset;
}

std::string CodeGenerator::getVariableAddress(const std::string& name) {
    if (symbolTable.find(name) == symbolTable.end()) {
        std::cerr << "Error: Undefined variable '" << name << "'" << std::endl;
        return "";
    }
    return "-" + std::to_string(symbolTable[name]) + "(%rbp)";
}

void CodeGenerator::generateFunctionPrologue(const std::string& funcName) {
    output << ".section .text" << std::endl;
    output << ".globl " << funcName << std::endl;
    output << funcName << ":" << std::endl;
    output << "    pushq %rbp" << std::endl;
    output << "    movq %rsp, %rbp" << std::endl;
    // 为局部变量预留栈空间
    if (stackOffset > 0) {
        output << "    subq $" << stackOffset << ", %rsp" << std::endl;
    }
}

void CodeGenerator::generateFunctionEpilogue() {
    output << "    leave" << std::endl;
    output << "    ret" << std::endl;
}

void CodeGenerator::visit(IntegerLiteral* node) {
    output << "    movq $" << node->value << ", %rax" << std::endl;
}

void CodeGenerator::visit(Identifier* node) {
    std::string address = getVariableAddress(node->name);
    if (!address.empty()) {
        output << "    movq " << address << ", %rax" << std::endl;
    }
}

void CodeGenerator::visit(BinaryExpression* node) {
    // 先计算右操作数并保存到临时内存位置
    node->right->accept(this);
    // 分配8字节的临时空间
    stackOffset += 8;
    int rightTempOffset = stackOffset; // 记住右操作数的位置
    output << "    movq %rax, -" << rightTempOffset << "(%rbp)" << std::endl;
    
    // 再计算左操作数
    node->left->accept(this);
    // 从临时内存位置加载右操作数
    output << "    movq -" << rightTempOffset << "(%rbp), %rbx" << std::endl;
    
    // 执行运算
    if (node->op == "+") {
        output << "    addq %rbx, %rax" << std::endl;
    } else if (node->op == "-") {
        output << "    subq %rbx, %rax" << std::endl;
    } else if (node->op == "*") {
        output << "    imulq %rbx, %rax" << std::endl;
    } else if (node->op == "/") {
        output << "    cqto" << std::endl;
        output << "    idivq %rbx" << std::endl;
    } else if (node->op == "%") {
        output << "    cqto" << std::endl;
        output << "    idivq %rbx" << std::endl;
        output << "    movq %rdx, %rax" << std::endl;
    } else if (node->op == "==") {
        output << "    cmpq %rbx, %rax" << std::endl;
        output << "    sete %al" << std::endl;
        output << "    movzbq %al, %rax" << std::endl;
    } else if (node->op == "!=") {
        output << "    cmpq %rbx, %rax" << std::endl;
        output << "    setne %al" << std::endl;
        output << "    movzbq %al, %rax" << std::endl;
    } else if (node->op == "<") {
        output << "    cmpq %rbx, %rax" << std::endl;
        output << "    setl %al" << std::endl;
        output << "    movzbq %al, %rax" << std::endl;
    } else if (node->op == ">") {
        output << "    cmpq %rbx, %rax" << std::endl;
        output << "    setg %al" << std::endl;
        output << "    movzbq %al, %rax" << std::endl;
    } else if (node->op == "<=") {
        output << "    cmpq %rbx, %rax" << std::endl;
        output << "    setle %al" << std::endl;
        output << "    movzbq %al, %rax" << std::endl;
    } else if (node->op == ">=") {
        output << "    cmpq %rbx, %rax" << std::endl;
        output << "    setge %al" << std::endl;
        output << "    movzbq %al, %rax" << std::endl;
    } else if (node->op == "&&") {
        output << "    testq %rax, %rax" << std::endl;
        output << "    setne %al" << std::endl;
        output << "    testq %rbx, %rbx" << std::endl;
        output << "    setne %bl" << std::endl;
        output << "    andb %bl, %al" << std::endl;
        output << "    movzbq %al, %rax" << std::endl;
    } else if (node->op == "||") {
        output << "    orq %rbx, %rax" << std::endl;
        output << "    testq %rax, %rax" << std::endl;
        output << "    setne %al" << std::endl;
        output << "    movzbq %al, %rax" << std::endl;
    }
}

void CodeGenerator::visit(UnaryExpression* node) {
    node->operand->accept(this);
    
    if (node->op == "-") {
        output << "    negq %rax" << std::endl;
    } else if (node->op == "!") {
        output << "    testq %rax, %rax" << std::endl;
        output << "    sete %al" << std::endl;
        output << "    movzbq %al, %rax" << std::endl;
    }
}

void CodeGenerator::visit(AssignmentExpression* node) {
    // 计算右操作数
    node->right->accept(this);
    
    // 存储到左操作数（变量）
    std::string address = getVariableAddress(node->left->name);
    if (!address.empty()) {
        output << "    movq %rax, " << address << std::endl;
    }
}

void CodeGenerator::visit(FunctionCall* node) {
    // 简单的函数调用处理
    if (node->name == "printf") {
        // 处理printf函数调用
        output << "    # printf function call" << std::endl;
    } else {
        // 处理参数
        for (int i = node->arguments.size() - 1; i >= 0; i--) {
            node->arguments[i]->accept(this);
            output << "    push %rax" << std::endl;
        }
        
        // 调用函数
        output << "    call " << node->name << std::endl;
        
        // 清理参数
        if (!node->arguments.empty()) {
            output << "    add $" << (node->arguments.size() * 8) << ", %rsp" << std::endl;
        }
    }
}

void CodeGenerator::visit(ExpressionStatement* node) {
    node->expression->accept(this);
}

void CodeGenerator::visit(VariableDeclaration* node) {
    // 处理普通变量声明
    for (const auto& name : node->names) {
        allocateVariable(name);
        output << "    # Variable declaration: " << node->type << " " << name << std::endl;
    }
    
    // 处理带初始化的变量声明
    for (const auto& initDecl : node->initDeclarators) {
        const std::string& name = initDecl.first;
        const auto& initExpr = initDecl.second;
        
        // 分配变量空间
        allocateVariable(name);
        output << "    # Variable declaration with initialization: " << node->type << " " << name << std::endl;
        
        // 如果有初始化表达式，生成初始化代码
        if (initExpr) {
            initExpr->accept(this);
            std::string address = getVariableAddress(name);
            if (!address.empty()) {
                output << "    movq %rax, " << address << std::endl;
            }
        }
    }
}

void CodeGenerator::visit(CompoundStatement* node) {
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
}

void CodeGenerator::visit(IfStatement* node) {
    std::string falseLabel = generateLabel("if_false");
    std::string endLabel = generateLabel("if_end");
    
    // 计算条件
    node->condition->accept(this);
    output << "    testq %rax, %rax" << std::endl;
    output << "    je " << falseLabel << std::endl;
    
    // then分支
    node->thenStmt->accept(this);
    output << "    jmp " << endLabel << std::endl;
    
    // else分支
    output << falseLabel << ":" << std::endl;
    if (node->elseStmt) {
        node->elseStmt->accept(this);
    }
    
    output << endLabel << ":" << std::endl;
}

void CodeGenerator::visit(WhileStatement* node) {
    std::string loopLabel = generateLabel("while_loop");
    std::string endLabel = generateLabel("while_end");
    
    output << loopLabel << ":" << std::endl;
    
    // 计算条件
    node->condition->accept(this);
    output << "    testq %rax, %rax" << std::endl;
    output << "    je " << endLabel << std::endl;
    
    node->body->accept(this);
    output << "    jmp " << loopLabel << std::endl;
    
    output << endLabel << ":" << std::endl;
}

void CodeGenerator::visit(ForStatement* node) {
    std::string loopLabel = generateLabel("for_loop");
    std::string updateLabel = generateLabel("for_update");
    std::string endLabel = generateLabel("for_end");
    
    // 初始化语法
    if (node->init) {
        node->init->accept(this);
    }
    
    output << loopLabel << ":" << std::endl;
    
    // 条件检测
    if (node->condition) {
        node->condition->accept(this);
        output << "    testq %rax, %rax" << std::endl;
        output << "    je " << endLabel << std::endl;
    }
    

    node->body->accept(this);
    
    // 更新表达式
    output << updateLabel << ":" << std::endl;
    if (node->update) {
        node->update->accept(this);
    }
    
    output << "    jmp " << loopLabel << std::endl;
    output << endLabel << ":" << std::endl;
}

void CodeGenerator::visit(ReturnStatement* node) {
    if (node->value) {
        node->value->accept(this);
    } else {
        output << "    movq $0, %rax" << std::endl;
    }
    output << "    leave" << std::endl;
    output << "    ret" << std::endl;
}

void CodeGenerator::visit(FunctionDefinition* node) {
    currentFunction = node->name;
    symbolTable.clear();
    stackOffset = 0;
    
    // 先生成函数标签
    output << ".section .text" << std::endl;
    output << ".globl " << node->name << std::endl;
    output << node->name << ":" << std::endl;
    output << "    pushq %rbp" << std::endl;
    output << "    movq %rsp, %rbp" << std::endl;
    
    // 处理参数
    int paramOffset = 16; 
    for (const auto& param : node->parameters) {
        symbolTable[param.second] = -paramOffset;
        paramOffset += 8;
    }
    
    // 生成函数体（这会计算需要的栈空间）
    if (node->body) {
        node->body->accept(this);
    }
    
    // 分配栈空间（如果需要）
    if (stackOffset > 0) {

        output << "    # Stack space allocated: " << stackOffset << " bytes" << std::endl;
    }
    
    // 生成函数结束标签
    std::string endLabel = generateLabel("func_end");
    output << endLabel << ":" << std::endl;
    

    output << "    # Default return (if no explicit return)" << std::endl;
    if (node->returnType != "void") {
        output << "    movq $0, %rax" << std::endl;
    }
    output << "    leave" << std::endl;
    output << "    ret" << std::endl;
    
    output << std::endl;
}

void CodeGenerator::visit(Program* node) {
    // 生成汇编文件头部
    output << "# Generated by C Compiler" << std::endl;
    output << std::endl;
    
    // 处理所有声明
    for (const auto& decl : node->declarations) {
        decl->accept(this);
    }
}

void CodeGenerator::generateAssembly(Program* program) {
    if (program) {
        program->accept(this);
    }
} 
 