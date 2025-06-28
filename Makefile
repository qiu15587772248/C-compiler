# C语言编译器 Makefile

# 编译器和工具设置
CXX = g++
FLEX = flex
BISON = bison
CXXFLAGS = -std=c++17 -g -Wall -O2
LDFLAGS = -lfl

# 目录设置
SRCDIR = src
BUILDDIR = build
TESTDIR = test

# 源文件
LEXER_L = $(SRCDIR)/lexer.l
PARSER_Y = $(SRCDIR)/parser.y
CPP_SOURCES = $(SRCDIR)/ast.cpp $(SRCDIR)/codegen.cpp $(SRCDIR)/semantic.cpp $(SRCDIR)/main.cpp
GENERATED_CPP = $(BUILDDIR)/lexer.yy.cpp $(BUILDDIR)/parser.tab.cpp
GENERATED_H = $(BUILDDIR)/parser.tab.h

# 目标文件
OBJECTS = $(BUILDDIR)/ast.o $(BUILDDIR)/codegen.o $(BUILDDIR)/semantic.o $(BUILDDIR)/main.o \
          $(BUILDDIR)/lexer.yy.o $(BUILDDIR)/parser.tab.o

# 最终目标
TARGET = $(BUILDDIR)/compiler

# 默认目标
all: $(TARGET)

# 创建目录
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# 生成语法分析器
$(BUILDDIR)/parser.tab.cpp $(BUILDDIR)/parser.tab.hpp: $(PARSER_Y) | $(BUILDDIR)
	$(BISON) -d -o $(BUILDDIR)/parser.tab.cpp $<

# 生成词法分析器（依赖parser.tab.hpp）
$(BUILDDIR)/lexer.yy.cpp: $(LEXER_L) $(BUILDDIR)/parser.tab.hpp | $(BUILDDIR)
	$(FLEX) -o $@ $<

# 编译规则
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(BUILDDIR) -c $< -o $@

$(BUILDDIR)/lexer.yy.o: $(BUILDDIR)/lexer.yy.cpp $(BUILDDIR)/parser.tab.hpp
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -I$(BUILDDIR) -c $< -o $@

$(BUILDDIR)/parser.tab.o: $(BUILDDIR)/parser.tab.cpp
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -I$(BUILDDIR) -c $< -o $@

# 链接生成最终程序
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

# 测试目标
test: $(TARGET)
	@echo "运行测试用例..."
	@if [ -f $(TESTDIR)/test1.c ]; then \
		echo "测试 test1.c:"; \
		$(TARGET) $(TESTDIR)/test1.c -o $(BUILDDIR)/test1.s; \
		echo ""; \
	fi
	@if [ -f $(TESTDIR)/test2.c ]; then \
		echo "测试 test2.c:"; \
		$(TARGET) $(TESTDIR)/test2.c -o $(BUILDDIR)/test2.s; \
		echo ""; \
	fi
	@if [ -f $(TESTDIR)/test3.c ]; then \
		echo "测试 test3.c:"; \
		$(TARGET) $(TESTDIR)/test3.c -o $(BUILDDIR)/test3.s; \
		echo ""; \
	fi

# 安装目标
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/c-compiler

# 清理目标
clean:
	rm -rf $(BUILDDIR)/*

distclean: clean
	rm -rf $(BUILDDIR)

# 调试信息
debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET)

# 帮助信息
help:
	@echo "可用目标:"
	@echo "  all      - 构建编译器 (默认)"
	@echo "  test     - 运行测试用例"
	@echo "  clean    - 清理构建文件"
	@echo "  distclean- 完全清理"
	@echo "  debug    - 构建调试版本"
	@echo "  install  - 安装到系统"
	@echo "  help     - 显示此帮助信息"

# 声明伪目标
.PHONY: all test clean distclean debug help install

# 依赖关系
$(BUILDDIR)/main.o: $(SRCDIR)/ast.h $(SRCDIR)/codegen.h $(SRCDIR)/semantic.h
$(BUILDDIR)/ast.o: $(SRCDIR)/ast.h
$(BUILDDIR)/codegen.o: $(SRCDIR)/ast.h $(SRCDIR)/codegen.h
$(BUILDDIR)/semantic.o: $(SRCDIR)/ast.h $(SRCDIR)/semantic.h 