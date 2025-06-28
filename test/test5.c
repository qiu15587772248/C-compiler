// 测试用例5: 复杂初始化表达式
int main() {
    int x = 5;
    int y = 3;
    int sum = x + y;
    int product = x * y;
    int complex = sum * product + x;  // (5+3) * (5*3) + 5 = 8*15+5 = 125
    return complex;
} 