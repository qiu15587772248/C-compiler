#include "ast.h"
#include "codegen.h"
#include "semantic.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

// 外部声明
extern int yyparse();
extern int yylex();
extern FILE* yyin;
extern Program* program_root;
extern int yylineno;
extern char* yytext;

// Token名称映射
const char* getTokenName(int token) {
    switch(token) {
        case 258: return "INTEGER_LITERAL";
        case 259: return "IDENTIFIER";
        case 260: return "INT";
        case 261: return "CHAR";
        case 262: return "FLOAT";
        case 263: return "DOUBLE";
        case 264: return "VOID";
        case 265: return "IF";
        case 266: return "ELSE";
        case 267: return "WHILE";
        case 268: return "FOR";
        case 269: return "RETURN";
        case 270: return "BREAK";
        case 271: return "CONTINUE";
        case 272: return "EQ";
        case 273: return "NE";
        case 274: return "LE";
        case 275: return "GE";
        case 276: return "AND";
        case 277: return "OR";
        case 278: return "INC";
        case 279: return "DEC";
        case 280: return "ERROR_TOKEN";
        case '+': return "+";
        case '-': return "-";
        case '*': return "*";
        case '/': return "/";
        case '%': return "%";
        case '=': return "=";
        case '<': return "<";
        case '>': return ">";
        case '!': return "!";
        case '(': return "(";
        case ')': return ")";
        case '{': return "{";
        case '}': return "}";
        case '[': return "[";
        case ']': return "]";
        case ';': return ";";
        case ',': return ",";
        case 0: return "EOF";
        default: return "UNKNOWN";
    }
}

// 打印 DFA 状态信息
void printDFAInfo() {
    std::cout << "\n=== 词法分析器DFA信息 ===" << std::endl;
    
    // 统计正则表达式规则数量
    int ruleCount = 0;
    int keywordCount = 8;  // int, char, float, double, void, if, else, while, for, return, break, continue
    int operatorCount = 8; // ==, !=, <=, >=, &&, ||, ++, --
    int singleCharCount = 13; // +, -, *, /, %, =, <, >, !, (, ), {, }, [, ], ;, ,
    int literalCount = 2;  // INTEGER, IDENTIFIER
    int commentCount = 2;  // /* */ 和 //
    int whitespaceCount = 2; // 空白字符和换行
    
    ruleCount = keywordCount + operatorCount + singleCharCount + literalCount + commentCount + whitespaceCount + 1; // +1 for error
    
    std::cout << "DFA状态机统计信息：" << std::endl;
    std::cout << "  总规则数: " << ruleCount << std::endl;
    std::cout << "  - 关键字规则: " << keywordCount << " 个" << std::endl;
    std::cout << "  - 操作符规则: " << operatorCount << " 个" << std::endl;
    std::cout << "  - 单字符规则: " << singleCharCount << " 个" << std::endl;
    std::cout << "  - 字面量规则: " << literalCount << " 个" << std::endl;
    std::cout << "  - 注释规则: " << commentCount << " 个" << std::endl;
    std::cout << "  - 空白字符规则: " << whitespaceCount << " 个" << std::endl;
    std::cout << "  - 错误处理规则: 1 个" << std::endl;
    
    // 估算状态数（基于规则复杂度）
    int estimatedStates = ruleCount * 2 + 10; // 粗略估算
    std::cout << "\n状态机结构：" << std::endl;
    std::cout << "  估算状态数: ~" << estimatedStates << " 个状态" << std::endl;
    std::cout << "  初始状态: 0" << std::endl;
    std::cout << "  终结状态: 多个（每种token类型对应一个）" << std::endl;
    

    std::cout << "=========================" << std::endl;
}

// 词法分析函数
void performLexicalAnalysis(const std::string& inputFile, bool showDFA = false) {
    yyin = fopen(inputFile.c_str(), "r");
    if (!yyin) {
        std::cerr << "错误: 无法打开输入文件 '" << inputFile << "'" << std::endl;
        return;
    }
    
    if (showDFA) {
        printDFAInfo();
    }
    
    std::cout << "\n=== 词法分析结果（Token序列） ===" << std::endl;
    std::cout << "行号\t词法单元\t\t词素\t\tToken值" << std::endl;
    std::cout << "----\t--------\t\t----\t\t-------" << std::endl;
    
    int token;
    yylineno = 1;
    int tokenCount = 0;
    int errorCount = 0;
    
    while ((token = yylex()) != 0) {
        if (token == 280) { // ERROR_TOKEN
            errorCount++;
            std::cout << yylineno << "\t" << getTokenName(token) << "\t\t" 
                      << yytext << "\t\t" << token << " (词法错误)" << std::endl;
            break;
        } else {
            std::cout << yylineno << "\t" << getTokenName(token) << "\t\t" 
                      << yytext << "\t\t" << token << std::endl;
            tokenCount++;
        }
    }
    
    std::cout << "\n词法分析统计：" << std::endl;
    std::cout << "有效Token数量: " << tokenCount << std::endl;
    if (errorCount > 0) {
        std::cout << "词法错误数量: " << errorCount << std::endl;
        std::cout << "✗ 词法分析失败" << std::endl;
    } else {
        std::cout << "✓ 词法分析成功" << std::endl;
    }
    std::cout << "=============================" << std::endl;
    fclose(yyin);
}

// 语法分析函数
bool performSyntaxAnalysis(const std::string& inputFile, bool printAST = false) {
    yyin = fopen(inputFile.c_str(), "r");
    if (!yyin) {
        std::cerr << "错误: 无法打开输入文件 '" << inputFile << "'" << std::endl;
        return false;
    }
    
    std::cout << "\n=== 语法分析过程 ===" << std::endl;
    std::cout << "正在进行语法分析..." << std::endl;
    
    // 初始化全局变量
    program_root = nullptr;
    yylineno = 1;
    
    // 执行语法分析
    int parseResult = yyparse();
    fclose(yyin);
    
    if (parseResult != 0) {
        std::cout << "\n语法分析失败！程序包含语法错误，无法继续进行语义分析。" << std::endl;
        std::cout << "请修复上述语法错误后重新编译。" << std::endl;
        return false;
    }
    
    if (!program_root) {
        std::cout << "错误: 未生成语法树" << std::endl;
        return false;
    }
    
    std::cout << "✓ 语法分析成功！抽象语法树构建完成。" << std::endl;
    
    // 打印AST
    if (printAST) {
        std::cout << "\n=== 抽象语法树（AST） ===" << std::endl;
        program_root->print();
        std::cout << "===========================" << std::endl;
    }
    
    return true;
}

// 语法分析函数
bool performSyntaxAnalysisQuiet(const std::string& inputFile) {
    yyin = fopen(inputFile.c_str(), "r");
    if (!yyin) {
        std::cerr << "错误: 无法打开输入文件 '" << inputFile << "'" << std::endl;
        return false;
    }
    
    // 初始化全局变量
    program_root = nullptr;
    yylineno = 1;
    
    // 执行语法分析
    int parseResult = yyparse();
    fclose(yyin);
    
    if (parseResult != 0) {
        std::cerr << "语法分析失败！程序包含语法错误，无法继续编译。" << std::endl;
        return false;
    }
    
    if (!program_root) {
        std::cerr << "错误: 未生成语法树" << std::endl;
        return false;
    }
    
    return true;
}

void printUsage(const char* progName) {
    std::cout << "用法: " << progName << " [选项] <输入文件>" << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -o <输出文件>  指定输出文件名" << std::endl;
    std::cout << "  -h, --help     显示帮助信息" << std::endl;
    std::cout << "  -v, --version  显示版本信息" << std::endl;
    std::cout << "  --tokens       仅进行词法分析，输出Token序列" << std::endl;
    std::cout << "  --tokens-dfa   词法分析 + DFA信息" << std::endl;
    std::cout << "  --ast          仅进行语法分析，输出抽象语法树" << std::endl;
    std::cout << "  --semantic     进行语义分析，输出语义信息" << std::endl;
    std::cout << "  --all-phases   展示所有分析阶段的成果" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << progName << " test.c -o test.s" << std::endl;
    std::cout << "  " << progName << " test.c --tokens" << std::endl;
    std::cout << "  " << progName << " test.c --ast" << std::endl;
    std::cout << "  " << progName << " test.c --semantic" << std::endl;
    std::cout << "  " << progName << " test.c --all-phases" << std::endl;
}

void printVersion() {
    std::cout << "编译原理课设编译器 v1.0" << std::endl;
    std::cout << "支持基本C语言语法，生成x86汇编代码" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile;
    bool tokensOnly = false;
    bool tokensDFA = false;
    bool astOnly = false;
    bool semanticOnly = false;
    bool allPhases = false;
    
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printVersion();
            return 0;
        } else if (strcmp(argv[i], "--tokens") == 0) {
            tokensOnly = true;
        } else if (strcmp(argv[i], "--tokens-dfa") == 0) {
            tokensDFA = true;
        } else if (strcmp(argv[i], "--ast") == 0) {
            astOnly = true;
        } else if (strcmp(argv[i], "--semantic") == 0) {
            semanticOnly = true;
        } else if (strcmp(argv[i], "--all-phases") == 0) {
            allPhases = true;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "错误: -o 选项需要指定输出文件名" << std::endl;
                return 1;
            }
        } else if (argv[i][0] != '-') {
            if (inputFile.empty()) {
                inputFile = argv[i];
            } else {
                std::cerr << "错误: 只能指定一个输入文件" << std::endl;
                return 1;
            }
        } else {
            std::cerr << "错误: 未知选项 " << argv[i] << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // 检查输入文件
    if (inputFile.empty()) {
        std::cerr << "错误: 请指定输入文件" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // 处理各种分析模式
    if (tokensOnly) {
        // 仅词法分析
        performLexicalAnalysis(inputFile, false);
        return 0;
    }
    
    if (tokensDFA) {
        // 词法分析 + DFA信息
        performLexicalAnalysis(inputFile, true);
        return 0;
    }
    
    if (astOnly) {
        // 仅语法分析
        if (performSyntaxAnalysis(inputFile, true)) {
            delete program_root;
            return 0;
        } else {
            return 1;
        }
    }
    
    if (semanticOnly) {
        // 仅语义分析
        if (performSyntaxAnalysis(inputFile, false)) {
            SemanticAnalyzer analyzer;
            bool success = analyzer.analyze(program_root);
            
            // 输出带语义信息的语法树
            analyzer.printSemanticTree(program_root);
            
            delete program_root;
            return success ? 0 : 1;
        } else {
            return 1;
        }
    }
    
    if (allPhases) {
        // 展示所有分析阶段
        std::cout << "=== 编译器各阶段分析成果展示 ===" << std::endl;
        
        // 1. 词法分析
        std::cout << "\n第一阶段：词法分析" << std::endl;
        performLexicalAnalysis(inputFile, true);
        
        // 2. 语法分析
        std::cout << "\n第二阶段：语法分析" << std::endl;
        if (!performSyntaxAnalysis(inputFile, true)) {
            std::cout << "\n=== 编译过程终止 ===" << std::endl;
            std::cout << "✗ 由于语法错误，编译过程无法继续。" << std::endl;
            std::cout << "请修复语法错误后重新编译。" << std::endl;
            return 1;
        }
        
        // 3. 语义分析
        std::cout << "\n第三阶段：语义分析" << std::endl;
        SemanticAnalyzer analyzer;
        bool semanticSuccess = analyzer.analyze(program_root);
        
        // 输出带语义信息的语法树
        analyzer.printSemanticTree(program_root);
        
        std::cout << "\n=== 所有分析阶段完成 ===" << std::endl;
        if (semanticSuccess) {
            std::cout << "✓ 所有分析阶段都成功通过！程序可以继续进行代码生成。" << std::endl;
        } else {
            std::cout << "✗ 语义分析阶段发现错误，请修复后重新编译。" << std::endl;
        }
        
        delete program_root;
        return semanticSuccess ? 0 : 1;
    }
    
    // 默认编译模式（生成汇编代码）
    // 设置默认输出文件名
    if (outputFile.empty()) {
        size_t pos = inputFile.find_last_of('.');
        if (pos != std::string::npos) {
            outputFile = inputFile.substr(0, pos) + ".s";
        } else {
            outputFile = inputFile + ".s";
        }
    }
    
    // 执行语法分析
    if (!performSyntaxAnalysisQuiet(inputFile)) {
        return 1;
    }
    
    // 执行语义分析
    SemanticAnalyzer analyzer;
    if (!analyzer.analyze(program_root, true)) {
        std::cerr << "语义分析失败，停止编译。" << std::endl;
        std::cerr << "请使用 --semantic 选项查看详细的语义错误信息。" << std::endl;
        delete program_root;
        return 1;
    }
    
    // 生成汇编代码
    std::cerr << "正在生成汇编代码..." << std::endl;
    
    // 直接输出到标准输出，不使用文件
    CodeGenerator codeGen(std::cout);
    codeGen.generateAssembly(program_root);
    
    std::cerr << "汇编代码生成成功！" << std::endl;
    
    // 清理内存
    delete program_root;
    
    return 0;
} 