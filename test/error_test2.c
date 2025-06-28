int main() {
    int a;
    int b = 10;
    
    // 错误1: 使用未声明的变量
    c = 5;
    
    // 错误2: 重复声明变量
    int a;
    
    // 错误3: 使用未初始化的变量
    int result = a + b;
    
    // 错误4: 调用未声明的函数
    foo();
    
    return 0;
} 