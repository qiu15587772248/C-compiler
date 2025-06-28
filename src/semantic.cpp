#include "semantic.h"
#include <iostream>
#include <iomanip>

// SymbolTable 实现
SymbolTable::SymbolTable() : currentScope(-1) {
    enterScope(); // 进入全局作用域
}

void SymbolTable::enterScope() {
    scopes.emplace_back();
    currentScope++;
}

void SymbolTable::exitScope() {
    if (currentScope > 0) {
        scopes.pop_back();
        currentScope--;
    }
}

bool SymbolTable::declare(const std::string& name, const std::string& type, const std::string& kind) {
    // 检查当前作用域是否已有同名符号
    if (lookupInCurrentScope(name) != nullptr) {
        return false; // 重复声明
    }
    
    scopes[currentScope][name] = std::unique_ptr<SymbolInfo>(new SymbolInfo(name, type, kind, currentScope));
    return true;
}

SymbolInfo* SymbolTable::lookup(const std::string& name) {
    // 从当前作用域向外查找
    for (int i = currentScope; i >= 0; i--) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) {
            return it->second.get();
        }
    }
    return nullptr;
}

SymbolInfo* SymbolTable::lookupInCurrentScope(const std::string& name) {
    if (currentScope >= 0) {
        auto it = scopes[currentScope].find(name);
        if (it != scopes[currentScope].end()) {
            return it->second.get();
        }
    }
    return nullptr;
}

void SymbolTable::print() const {
    std::cout << "\n=== 符号表信息 ===" << std::endl;
    for (int scope = 0; scope <= currentScope; scope++) {
        std::cout << "作用域 " << scope << ":" << std::endl;
        std::cout << std::setw(15) << "符号名" << std::setw(10) << "类型" 
                  << std::setw(12) << "种类" << std::setw(12) << "已初始化" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        
        for (const auto& pair : scopes[scope]) {
            const auto& name = pair.first;
            const auto& symbol = pair.second;
            std::cout << std::setw(15) << symbol->name 
                      << std::setw(10) << symbol->type
                      << std::setw(12) << symbol->kind
                      << std::setw(12) << (symbol->isInitialized ? "是" : "否") << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "=================" << std::endl;
}

// SemanticAnalyzer 实现
bool SemanticAnalyzer::analyze(Program* program, bool silent) {
    errors.clear();
    warnings.clear();
    
    if (!silent) {
        std::cout << "\n=== 语义分析过程 ===" << std::endl;
        std::cout << "开始语义分析..." << std::endl;
    }
    
    // 访问程序根节点
    program->accept(this);
    
    if (!silent) {
        std::cout << "语义分析完成。" << std::endl;
        // 打印分析结果
        printResults();
    }
    
    return !hasErrors();
}

void SemanticAnalyzer::printResults() const {
    std::cout << "\n=== 语义分析结果 ===" << std::endl;
    
    // 打印错误信息
    if (!errors.empty()) {
        std::cout << "\n语义错误 (" << errors.size() << " 个):" << std::endl;
        for (const auto& error : errors) {
            std::cout << "[" << error.errorType << "]";
            if (error.line > 0) {
                std::cout << " 行 " << error.line;
                if (error.column > 0) {
                    std::cout << ":" << error.column;
                }
            }
            if (!error.context.empty()) {
                std::cout << " 在 " << error.context;
            }
            std::cout << ": " << error.message << std::endl;
        }
    }
    
    // 打印警告信息
    if (!warnings.empty()) {
        std::cout << "\n警告 (" << warnings.size() << " 个):" << std::endl;
        for (const auto& warning : warnings) {
            std::cout << "警告: " << warning << std::endl;
        }
    }
    
    if (errors.empty() && warnings.empty()) {
        std::cout << "\n✓ 语义分析通过，未发现错误或警告。" << std::endl;
    }
    
    std::cout << "====================" << std::endl;
}

void SemanticAnalyzer::printSemanticTree(Program* program) const {
    std::cout << "\n=== 带语义信息的抽象语法树 ===" << std::endl;
    program->printWithSemantics();
    std::cout << "================================" << std::endl;
}

void SemanticAnalyzer::addError(const std::string& message, const std::string& errorType, const std::string& context) {
    std::string finalContext = context.empty() ? currentContext : context;
    errors.emplace_back(message, errorType, currentLine, 0, finalContext);
}

void SemanticAnalyzer::addWarning(const std::string& message) {
    warnings.push_back(message);
}

TypeInfo SemanticAnalyzer::getExpressionType(Expression* expr) {
    TypeInfo oldType = currentExpressionType;
    expr->accept(this);
    TypeInfo result = currentExpressionType;
    currentExpressionType = oldType;
    return result;
}

bool SemanticAnalyzer::isValidBinaryOperation(const std::string& op, const TypeInfo& left, const TypeInfo& right) {
    if (!left.isValid || !right.isValid) return false;
    
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        return left.isNumeric() && right.isNumeric();
    } else if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
        return left.isNumeric() && right.isNumeric();
    } else if (op == "&&" || op == "||") {
        return true; // 任何类型都可以用于逻辑运算
    }
    
    return false;
}

bool SemanticAnalyzer::isValidUnaryOperation(const std::string& op, const TypeInfo& operand) {
    if (!operand.isValid) return false;
    
    if (op == "-" || op == "+") {
        return operand.isNumeric();
    } else if (op == "!") {
        return true; // 任何类型都可以用于逻辑非运算
    }
    
    return false;
}

std::string SemanticAnalyzer::getResultType(const std::string& op, const TypeInfo& left, const TypeInfo& right) {
    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=" ||
        op == "&&" || op == "||") {
        return "int"; // 比较和逻辑运算返回整型
    }
    
    // 算术运算的类型提升规则
    if (left.baseType == "double" || right.baseType == "double") return "double";
    if (left.baseType == "float" || right.baseType == "float") return "float";
    return "int";
}

// 访问者模式实现
void SemanticAnalyzer::visit(IntegerLiteral* node) {
    currentExpressionType = TypeInfo("int");
    
    // 填充语义信息
    node->semanticInfo.type = "int";
    node->semanticInfo.symbolKind = "literal";
    node->semanticInfo.isInitialized = true;
}

void SemanticAnalyzer::visit(Identifier* node) {
    currentLine = node->lineNumber;
    setCurrentContext("标识符 '" + node->name + "'");
    
    SymbolInfo* symbol = symbolTable.lookup(node->name);
    if (!symbol) {
        addError("未声明的标识符 '" + node->name + "'", "未声明错误", "标识符使用");
        currentExpressionType = TypeInfo("", false);
        
        // 填充错误的语义信息
        node->semanticInfo.hasSemanticError = true;
        node->semanticInfo.errorMessage = "未声明的标识符";
        return;
    }
    
    if (symbol->kind == "variable" && !symbol->isInitialized) {
        addWarning("使用了未初始化的变量 '" + node->name + "'");
    }
    
    currentExpressionType = TypeInfo(symbol->type);
    
    // 填充语义信息
    node->semanticInfo.type = symbol->type;
    node->semanticInfo.symbolKind = symbol->kind;
    node->semanticInfo.isInitialized = symbol->isInitialized;
    node->semanticInfo.scopeLevel = symbol->scopeLevel;
}

void SemanticAnalyzer::visit(BinaryExpression* node) {
    currentLine = node->lineNumber;
    setCurrentContext("二元表达式 '" + node->op + "'");
    
    TypeInfo leftType = getExpressionType(node->left.get());
    TypeInfo rightType = getExpressionType(node->right.get());
    
    if (!isValidBinaryOperation(node->op, leftType, rightType)) {
        addError("无效的二元运算: " + leftType.baseType + " " + node->op + " " + rightType.baseType, "类型错误", "二元运算表达式");
        currentExpressionType = TypeInfo("", false);
        
        // 填充错误的语义信息
        node->semanticInfo.hasSemanticError = true;
        node->semanticInfo.errorMessage = "无效的二元运算";
        return;
    }
    
    std::string resultType = getResultType(node->op, leftType, rightType);
    currentExpressionType = TypeInfo(resultType);
    
    // 填充语义信息
    node->semanticInfo.type = resultType;
    node->semanticInfo.symbolKind = "expression";
    node->semanticInfo.isInitialized = true;
}

void SemanticAnalyzer::visit(UnaryExpression* node) {
    TypeInfo operandType = getExpressionType(node->operand.get());
    
    if (!isValidUnaryOperation(node->op, operandType)) {
        addError("无效的一元运算: " + node->op + operandType.baseType);
        currentExpressionType = TypeInfo("", false);
        return;
    }
    
    currentExpressionType = operandType;
}

void SemanticAnalyzer::visit(AssignmentExpression* node) {
    currentLine = node->lineNumber;
    setCurrentContext("赋值表达式");
    
    // 检查左值是否已声明
    SymbolInfo* symbol = symbolTable.lookup(node->left->name);
    if (!symbol) {
        addError("未声明的变量 '" + node->left->name + "'", "未声明错误", "赋值表达式左值");
        currentExpressionType = TypeInfo("", false);
        return;
    }
    
    if (symbol->kind != "variable" && symbol->kind != "parameter") {
        addError("不能给非变量 '" + node->left->name + "' 赋值", "赋值错误", "赋值表达式");
        currentExpressionType = TypeInfo("", false);
        return;
    }
    
    // 检查右值类型
    TypeInfo rightType = getExpressionType(node->right.get());
    TypeInfo leftType(symbol->type);
    
    if (!rightType.canAssignTo(leftType)) {
        addError("类型不匹配: 不能将 " + rightType.baseType + " 赋值给 " + leftType.baseType, "类型错误", "赋值表达式");
        currentExpressionType = TypeInfo("", false);
        return;
    }
    
    // 标记变量已初始化
    symbol->isInitialized = true;
    currentExpressionType = leftType;
}

void SemanticAnalyzer::visit(FunctionCall* node) {
    currentLine = node->lineNumber;
    setCurrentContext("函数调用 '" + node->name + "'");
    
    SymbolInfo* symbol = symbolTable.lookup(node->name);
    if (!symbol) {
        addError("未声明的函数 '" + node->name + "'", "未声明错误", "函数调用");
        currentExpressionType = TypeInfo("", false);
        return;
    }
    
    if (symbol->kind != "function") {
        addError("'" + node->name + "' 不是函数", "类型错误", "函数调用");
        currentExpressionType = TypeInfo("", false);
        return;
    }
    
    // 假设函数调用类型正确
    currentExpressionType = TypeInfo(symbol->type);
}

void SemanticAnalyzer::visit(ExpressionStatement* node) {
    if (node->expression) {
        node->expression->accept(this);
    }
}

void SemanticAnalyzer::visit(VariableDeclaration* node) {
    currentLine = node->lineNumber;
    setCurrentContext("变量声明");
    
    // 处理简单声明
    for (const auto& name : node->names) {
        if (!symbolTable.declare(name, node->type, "variable")) {
            addError("重复声明变量 '" + name + "'", "重复声明错误", "变量声明");
        }
    }
    
    // 处理带初始化的声明
    for (const auto& pair : node->initDeclarators) {
        const auto& name = pair.first;
        const auto& expr = pair.second;
        if (!symbolTable.declare(name, node->type, "variable")) {
            addError("重复声明变量 '" + name + "'", "重复声明错误", "变量声明");
            continue;
        }
        
        if (expr) {
            TypeInfo initType = getExpressionType(expr.get());
            TypeInfo varType(node->type);
            
            if (!initType.canAssignTo(varType)) {
                addError("初始化类型不匹配: 不能将 " + initType.baseType + " 赋值给 " + varType.baseType, "类型错误", "变量初始化");
            } else {
                // 标记变量已初始化
                SymbolInfo* symbol = symbolTable.lookup(name);
                if (symbol) {
                    symbol->isInitialized = true;
                }
            }
        }
    }
}

void SemanticAnalyzer::visit(CompoundStatement* node) {
    symbolTable.enterScope();
    
    for (const auto& stmt : node->statements) {
        stmt->accept(this);
    }
    
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(IfStatement* node) {
    node->condition->accept(this);
    node->thenStmt->accept(this);
    if (node->elseStmt) {
        node->elseStmt->accept(this);
    }
}

void SemanticAnalyzer::visit(WhileStatement* node) {
    node->condition->accept(this);
    node->body->accept(this);
}

void SemanticAnalyzer::visit(ForStatement* node) {
    if (node->init) {
        node->init->accept(this);
    }
    if (node->condition) {
        node->condition->accept(this);
    }
    if (node->update) {
        node->update->accept(this);
    }
    node->body->accept(this);
}

void SemanticAnalyzer::visit(ReturnStatement* node) {
    hasReturnStatement = true;
    
    if (node->value) {
        TypeInfo returnType = getExpressionType(node->value.get());
        TypeInfo expectedType(currentFunctionReturnType);
        
        if (!returnType.canAssignTo(expectedType)) {
            addError("返回类型不匹配: 期望 " + expectedType.baseType + 
                    ", 实际 " + returnType.baseType);
        }
    } else if (currentFunctionReturnType != "void") {
        addError("非void函数必须返回值");
    }
}

void SemanticAnalyzer::visit(FunctionDefinition* node) {
    currentLine = node->lineNumber;
    setCurrentContext("函数定义 '" + node->name + "'");
    
    // 声明函数
    if (!symbolTable.declare(node->name, node->returnType, "function")) {
        addError("重复声明函数 '" + node->name + "'", "重复声明错误", "函数定义");
    }
    
    // 进入函数作用域
    symbolTable.enterScope();
    currentFunctionReturnType = node->returnType;
    hasReturnStatement = false;
    
    // 声明参数
    for (const auto& param : node->parameters) {
        const auto& type = param.first;
        const auto& name = param.second;
        if (!symbolTable.declare(name, type, "parameter")) {
            addError("重复声明参数 '" + name + "'", "重复声明错误", "函数参数");
        } else {
            // 参数默认已初始化
            SymbolInfo* symbol = symbolTable.lookup(name);
            if (symbol) {
                symbol->isInitialized = true;
            }
        }
    }
    
    // 分析函数体
    node->body->accept(this);
    
    // 检查返回语句
    if (node->returnType != "void" && !hasReturnStatement) {
        addWarning("函数 '" + node->name + "' 可能没有返回值");
    }
    
    // 退出函数作用域
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(Program* node) {
    for (const auto& declaration : node->declarations) {
        declaration->accept(this);
    }
} 