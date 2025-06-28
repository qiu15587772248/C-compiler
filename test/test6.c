// 测试用例6: For循环测试
int main() {
    int sum = 0;
    int i;
    
    for (i = 1; i <= 5; i = i + 1) {
        sum = sum + i;
    }
    
    return sum;  // 期望返回15 (1+2+3+4+5)
} 