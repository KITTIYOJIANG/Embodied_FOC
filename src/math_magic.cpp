#include <iostream>

// 1. 施加魔法：constexpr 告诉编译器“这个函数可以在编译时跑完”
constexpr long long calculate_factorial(int n) {
    long long result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

int main() {
    std::cout << "--- 编译期魔法测试启动 ---\n";

    // 2. 终极考验：static_assert (静态断言)
    // 它是 C++ 中最恐怖的监工。它不仅要求判断条件为真，而且要求条件必须在【编译时】就能算出来！
    // 如果 calculate_factorial 被拖延到了运行期，编译器在编译这行代码时就会直接报错红牌罚下！
    
    constexpr long long f10 = calculate_factorial(10);
    static_assert(f10 == 3628800, "计算错误，或者没有在编译期执行！");

    std::cout << "10 的阶乘是: " << f10 << "\n";
    std::cout << "当你看到这行字时，上面的阶乘根本没有被计算，直接读取了常量答案。\n";

    return 0;
}