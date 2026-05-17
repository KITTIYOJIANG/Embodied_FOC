/**
 * @file    demo_loop_unroller.cpp
 * @brief   Demonstrates compile-time loop unrolling using LoopUnroller<>
 *          for branchless 3-phase current filtering.
 *
 * Build:
 *   cmake -B build && cmake --build build
 *   ./build/demo_loop_unroller
 */

#include <iostream>
#include "compile_time_utils.h"

/**
 * @brief  Demonstrates compile-time loop unrolling on a 3-element array.
 *
 * The LoopUnroller<3> template forces the compiler to physically emit
 * three sequential multiply instructions, eliminating loop-branch
 * overhead — beneficial in FOC current-sense filtering at >10 kHz.
 */
int main() {
    float currents[3] = {1.2f, -0.6f, -0.6f};
    constexpr float filter_gain = 0.9f;

    std::cout << "--- Embodied_FOC_Core: Loop Unroller Demo ---\n";

    // Compile-time unrolled loop — zero branch instructions emitted
    LoopUnroller<3>::execute([&currents](size_t i) {
        currents[i] *= filter_gain;
        std::cout << "  [Unrolled] currents[" << i << "] = "
                  << currents[i] << "\n";
    });

    return 0;
}