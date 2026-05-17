/**
 * @file    tensor.h
 * @brief   Lightweight, non-copyable tensor container with move semantics.
 *
 * Designed for zero-copy buffer management in real-time FOC pipelines.
 * The class is move-only: deep copies are forbidden at compile time to
 * prevent accidental memory thrashing in high-frequency control loops.
 */
#pragma once

#include <memory>
#include <cstddef>

/**
 * @brief  A minimal, non-copyable float tensor with move semantics.
 *
 * Intended for passing inference results (e.g., from ONNX runtime)
 * to the FOC control pipeline without heap fragmentation or double-free.
 *
 * Ownership rule (single-owner pattern):
 *   - Copy construction/assignment are deleted.
 *   - Move transfers the internal buffer efficiently (O(1)).
 *
 * @note    Compatible with RTOS environments: no dynamic allocations
 *          after construction, no iostream dependency.
 */
class Tensor {
private:
    std::unique_ptr<float[]> data_;  ///< Heap-allocated float array
    size_t size_;                    ///< Number of elements

public:
    /**
     * @brief Construct a tensor with given number of elements.
     * @param size  Number of floats to allocate (size > 0).
     */
    explicit Tensor(size_t size)
        : size_(size)
        , data_(std::make_unique<float[]>(size)) {}

    /** @brief Copy construction is forbidden — use move instead. */
    Tensor(const Tensor&) = delete;

    /** @brief Copy assignment is forbidden — use move instead. */
    Tensor& operator=(const Tensor&) = delete;

    /**
     * @brief Move constructor. Transfers ownership with O(1) complexity.
     * @param other  Source tensor; left in valid empty state afterwards.
     *
     * After the move, `other.data_` is null and `other.size_` is 0.
     */
    Tensor(Tensor&& other) noexcept
        : size_(other.size_)
        , data_(std::move(other.data_)) {
        other.size_ = 0;
    }

    /** @brief Destructor. Releases the internal buffer automatically. */
    ~Tensor() = default;
};