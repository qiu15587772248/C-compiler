# C语言编译器


## 📋 项目概述

本项目实现了一个完整的C语言编译器，支持基本的C语言语法，并能生成x86汇编代码。

### 🏗️ 技术架构
- **词法分析**: 使用Flex生成词法分析器
- **语法分析**: 使用Bison生成语法分析器
- **语法树**: 构建抽象语法树(AST)
- **代码生成**: 使用访问者模式生成x86汇编代码

### ✨ 支持的语言特性
- ✅ 变量声明和赋值
- ✅ **变量声明时初始化**: `int a = 5;` 
- ✅ 算术运算: `+`, `-`, `*`, `/`, `%`
- ✅ 比较运算: `<`, `>`, `<=`, `>=`, `==`, `!=`
- ✅ 逻辑运算: `&&`, `||`, `!`
- ✅ 控制结构: `if-else`, `while`循环, `for`循环
- ✅ 函数定义和返回值
- ✅ 生成x86-64汇编代码
- ✅ 混合声明方式支持

### ⚠️ 语法限制
- 不支持数组和指针
- 不支持字符串和浮点数
- 不支持结构体和联合体


## 🚀 快速开始

### 环境要求
- Windows 10/11
- MSYS2环境
- GCC编译器
- Flex词法分析器生成工具
- Bison语法分析器生成工具

### 安装MSYS2工具
```bash
# 在MSYS2中安装必要工具
pacman -S gcc flex bison make
```

## 📁 项目结构

```

├── src/                    # 源代码目录
│   ├── ast.h/ast.cpp      # 抽象语法树定义和实现
│   ├── codegen.h/codegen.cpp  # 代码生成器
│   ├── lexer.l            # Flex词法分析器定义
│   ├── parser.y           # Bison语法分析器定义
│   └── main.cpp           # 主程序
├── build/                 # 编译输出目录
│   └── compiler.exe       # 编译器可执行文件
├── test/                  # 测试用例目录
│   ├── test1.c           # 基本算术运算测试
│   ├── test2.c           # 条件语句测试
│   ├── test3.c           # while循环测试
│   ├── test4.c           # 变量初始化测试
│   ├── test5.c           # 复杂初始化表达式测试
│   └── test6.c           # for循环测试
├── Makefile              # 构建配置文件
└── README.md             # 项目说明文档
```

## 🛠️ 构建和使用

### 1. 构建编译器

```bash
# 进入项目目录
cd /e/编译原理课设

# 清理并构建编译器
make clean && make

# 检查编译器是否构建成功
ls -la build/compiler.exe
```

### 2. 使用编译器

#### 基本语法
```bash
./build/compiler <输入文件.c> -o <输出文件.s>
```

#### 编译测试文件
```bash
# 编译test1.c（基本算术运算）
./build/compiler test/test1.c -o test1_output.s

# 编译test2.c（条件语句）
./build/compiler test/test2.c -o test2_output.s

# 编译test3.c（循环语句）
./build/compiler test/test3.c -o test3_output.s
```

#### 编译自定义C文件
```bash
# 编译任意C文件
./build/compiler your_file.c -o output.s
```

### 3. 生成可执行文件

```bash
# 使用GCC汇编和链接
gcc test1_output.s -o test1_program.exe
gcc test2_output.s -o test2_program.exe
gcc test3_output.s -o test3_program.exe
```

### 4. 运行程序

```bash
# 运行程序并查看返回值
./test1_program.exe
echo "退出码: $?"

./test2_program.exe
echo "退出码: $?"

./test3_program.exe
echo "退出码: $?"
```

## 🔍 调试和查看

### 查看源文件内容
```bash
cat test/test1.c
cat test/test2.c
cat test/test3.c
```

### 查看生成的汇编代码
```bash
cat test1_output.s
cat test2_output.s
cat test3_output.s
```

### 使用AST调试模式
```bash
# 显示抽象语法树
./build/compiler test/test1.c -o test1_output.s --ast
```

## 🧪 测试用例

### Test1.c - 基本算术运算
```c
int main() {
    int a, b, c;
    a = 10;
    b = 20;
    c = a + b * 2;  // 结果: 50
    return c;
}
```

### Test2.c - 条件语句
```c
int main() {
    int x, y, max;
    x = 15;
    y = 25;
    
    if (x > y) {
        max = x;
    } else {
        max = y;  // 结果: 25
    }
    
    return max;
}
```

### Test3.c - While循环
```c
int main() {
    int i, sum;
    i = 1;
    sum = 0;
    
    while (i <= 10) {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;  // 结果: 55 (1+2+...+10)
}
```

### Test4.c - 变量初始化
```c
int main() {
    int a = 10;
    int b = 20;
    int c = a + b;  // 结果: 30
    return c;
}
```

### Test5.c - 复杂初始化表达式
```c
int main() {
    int x = 5;
    int y = 3;
    int sum = x + y;
    int product = x * y;
    int complex = sum * product + x;  // 结果: 125
    return complex;
}
```

### Test6.c - For循环
```c
int main() {
    int sum = 0;
    int i;
    
    for (i = 1; i <= 5; i = i + 1) {
        sum = sum + i;
    }
    
    return sum;  // 结果: 15 (1+2+3+4+5)
}
```

## 📋 完整测试流程

### 单个文件测试
```bash
echo "=== 测试test1.c ==="
cat test/test1.c
echo ""

./build/compiler test/test1.c -o test1.s
echo "生成的汇编代码："
cat test1.s
echo ""

gcc test1.s -o test1.exe
./test1.exe
result=$?
echo "程序返回值: $result (期望: 50)"

if [ $result -eq 50 ]; then
    echo "✅ 测试通过！"
else
    echo "❌ 测试失败！"
fi
```

### 批量测试
```bash
# 使用Makefile的测试功能
make test
```

### 快速测试脚本
```bash
# 一行命令完成编译->汇编->运行
./build/compiler test/test1.c -o temp.s && gcc temp.s -o temp.exe && ./temp.exe && echo "返回值: $?" && rm -f temp.s temp.exe
```

## 🗑️ 清理操作

### 清理编译器构建文件
```bash
make clean          # 清理build目录
make distclean      # 完全清理
```

### 清理测试生成的文件
```bash
# 删除所有.s汇编文件
rm -f *.s

# 删除所有.exe可执行文件
rm -f *.exe

# 一次性清理所有临时文件
rm -f *.s *.exe test*.s test*.exe
```

## 🎯 Makefile命令

| 命令 | 功能 | 说明 |
|------|------|------|
| `make` 或 `make all` | 构建编译器 | 默认目标，完整构建流程 |
| `make clean` | 清理构建文件 | 删除build目录中的所有文件 |
| `make test` | 运行测试 | 编译所有测试用例 |
| `make debug` | 调试版本 | 添加调试信息编译 |
| `make help` | 显示帮助 | 列出所有可用命令 |
| `make distclean` | 完全清理 | 彻底清理所有生成文件 |

## 🔧 使用模板

保存以下模板，每次使用时复制粘贴：

```bash
# 编译器测试指令模板
cd /e/编译原理课设
make clean && make

# 测试1: 基本算术运算 (期望返回50)
echo "=== 测试1: 基本算术运算 ==="
./build/compiler test/test1.c > test1.s
echo "生成的汇编代码前20行:"
head -20 test1.s
echo "汇编并运行:"
gcc test1.s -o test1.exe
./test1.exe
echo "返回值: $?"
echo

# 测试2: 条件语句 (期望返回25)
echo "=== 测试2: 条件语句 ==="
./build/compiler test/test2.c > test2.s
gcc test2.s -o test2.exe
./test2.exe
echo "返回值: $?"
echo

# 测试3: While循环 (期望返回55)
echo "=== 测试3: While循环 ==="
./build/compiler test/test3.c > test3.s
gcc test3.s -o test3.exe
./test3.exe
echo "返回值: $?"
echo

# 测试4: 变量初始化 (期望返回30)
echo "=== 测试4: 变量初始化 ==="
./build/compiler test/test4.c > test4.s
gcc test4.s -o test4.exe
./test4.exe
echo "返回值: $?"
echo

# 测试5: 复杂初始化表达式 (期望返回125)
echo "=== 测试5: 复杂初始化表达式 ==="
./build/compiler test/test5.c > test5.s
gcc test5.s -o test5.exe
./test5.exe
echo "返回值: $?"
echo

# 测试6: For循环 (期望返回15)
echo "=== 测试6: For循环 ==="
./build/compiler test/test6.c > test6.s
gcc test6.s -o test6.exe
./test6.exe
echo "返回值: $?"
echo
```

## 💡 使用技巧

### 命令历史
```bash
history          # 查看命令历史
!!              # 重复上一个命令
Ctrl+R          # 搜索历史命令
```

### 词法、语法、语义一键测试
```bash
./build/compiler test/test1.c --all-phases
```

## 🚨 常见问题

### 编译器构建失败
```bash
# 检查依赖工具
which flex bison gcc

# 重新安装工具
pacman -S flex bison gcc
```

### 权限问题
```bash
# 修改权限
chmod +x build/compiler.exe
```

### PowerShell环境问题
- 建议使用MSYS2终端而不是PowerShell
- 编译器依赖Unix风格的工具链

## 📚 开发说明

### 编译流程
1. **词法分析**: `lexer.l` → `lexer.yy.cpp`
2. **语法分析**: `parser.y` → `parser.tab.cpp`
3. **AST构建**: 构建抽象语法树
4. **代码生成**: 生成x86汇编代码

### 依赖关系
- `lexer.l` 依赖 `parser.y` 生成的头文件
- 所有源文件编译成目标文件
- 链接时需要 `-lfl` 库

## 🎓 学习价值

这个项目展示了：
- ✅ 完整的编译器前端实现
- ✅ Flex和Bison工具的使用
- ✅ 抽象语法树的设计和实现
- ✅ 访问者模式在编译器中的应用
- ✅ x86汇编代码生成技术
- ✅ 现代编译器构建工具链

##  技术支持

如果遇到问题，请检查：
1. MSYS2环境是否正确安装
2. 必要工具是否已安装 (gcc, flex, bison)
3. 项目目录结构是否完整
4. 是否在正确的目录下执行命令

---

**项目作者**: 陈锦秋
**开发环境**: Windows + MSYS2  
**编程语言**: C++
**核心工具**: Flex, Bison  
**目标平台**: x86-64 
