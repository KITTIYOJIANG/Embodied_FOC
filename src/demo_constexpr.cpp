/**
 * @file    demo_constexpr.cpp
 * @brief   Demonstrates compile-time factorial computation using constexpr.
 *
 * All arithmetic is evaluated by the compiler at build time — the resulting
 * binary contains only the final constant. This zero-runtime-overhead
 * pattern is critical for FOC lookup tables (sin/cos, arctan) and
 * calibration coefficients.
 *
 * Build:
 *   cmake -B build && cmake --build build
 *   ./build/demo_constexpr
 */

#include <iostream>

/**
 * @brief Compute factorial of n at compile time.
 * @param n  Integer input (0 <= n <= 20 for long long range).
 * @return   n! as a 64-bit integer.
 *
 * Declared constexpr, so when called with a constant expression,
 * the compiler evaluates it at build time.
 */
constexpr long long calculate_factorial(int n) {
    long long result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

int main() {
    std::cout << "--- Embodied_FOC_Core: Compile-Time Computation Demo ---\n";

    // This forces the compiler to evaluate at build time:
    // if calculate_factorial(10) were not constexpr-compatible,
    // static_assert would produce a hard compile error.
    constexpr long long f10 = calculate_factorial(10);
    static_assert(f10 == 3628800,
                  "calculate_factorial(10) must equal 3,628,800");

    std::cout << "10! = " << f10 << "\n";
    std::cout << "The value above was computed at compile time — "
              << "zero runtime cost.\n";

    return 0;
}