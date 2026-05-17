/**
 * @file    compile_time_utils.h
 * @brief   Compile-time utilities for FOC core, including loop unrolling
 *          and constexpr helpers. All code here runs at compile time —
 *          zero runtime overhead in the final binary.
 * @note    Part of Embodied_FOC_Core — use with any C++17 compliant toolchain.
 */
#pragma once

/**
 * @brief  Compile-time loop unroller with tail-anchored recursion.
 *
 * Forces the compiler to physically replicate the loop body `N` times,
 * eliminating branch prediction overhead — critical for high-frequency
 * FOC control loops (e.g., 3-phase current scaling, PI iteration).
 *
 * @tparam Index  Number of iterations (compile-time constant, >= 1)
 *
 * Usage:
 * @code
 * float currents[3] = {1.2f, -0.6f, -0.6f};
 * LoopUnroller<3>::execute([&currents](size_t i) {
 *     currents[i] *= 0.9f;  // filter gain
 * });
 * @endcode
 */
template<size_t Index>
struct LoopUnroller {
    template<typename Action>
    static inline void execute(Action&& action) {
        // Recursively unroll all previous indices first
        LoopUnroller<Index - 1>::execute(action);
        // Then execute the current index
        action(Index - 1);
    }
};

/**
 * @brief Specialization: base case for LoopUnroller.
 *
 * When Index reaches 0, no action is performed — this is a NO-OP,
 * ensuring each index [0, N-1] is called exactly once.
 */
template<>
struct LoopUnroller<0> {
    template<typename Action>
    static inline void execute(Action&& /*unused*/) {
        /* Base case — do nothing, exit recursion */
    }
};
