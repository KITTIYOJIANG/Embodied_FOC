/**
 * @file    double_buffer.h
 * @brief   Lock-free (via mutex) double-buffer for safe data exchange
 *          between asynchronous producer/consumer threads.
 *
 * Typical use-case in FOC:
 *   - Back-buffer:   Vision / perception thread writes new state (slow).
 *   - Front-buffer:  FOC control thread reads latest state (fast, ~10 kHz).
 *   - swapBuffers(): Atomic pointer swap under mutex — O(1).
 */
#pragma once

#include <memory>
#include <mutex>

/**
 * @brief  Thread-safe double-buffer with mutex-guarded swap.
 *
 * @tparam T  Element type (e.g., float, int, or a packed struct).
 *
 * Usage pattern:
 * @code
 * DoubleBuffer<float> buf(3);               // 3 float elements
 * float* back = buf.getBackBuffer();        // producer
 * back[0] = ia; back[1] = ib; back[2] = ic;
 * buf.swapBuffers();                        // publish
 *
 * float* front = buf.getFrontBuffer();      // consumer
 * float id = front[0];                      // read latest
 * @endcode
 */
template<typename T>
class DoubleBuffer {
private:
    std::unique_ptr<T[]> front_buffer_; ///< Buffer visible to consumer (read)
    std::unique_ptr<T[]> back_buffer_;  ///< Buffer writable by producer (write)
    std::mutex mtx_;                    ///< Guards swapBuffers()

public:
    /**
     * @brief Construct a double-buffer with `size` elements per buffer.
     * @param size  Number of elements (both front and back).
     */
    explicit DoubleBuffer(size_t size)
        : front_buffer_(std::make_unique<T[]>(size))
        , back_buffer_(std::make_unique<T[]>(size)) {}

    /**
     * @brief Atomically swap front and back buffers.
     *
     * The swap is O(1) — only raw pointers are exchanged under a mutex lock.
     * Designed to be called from the producer (writer) thread after
     * the back-buffer has been fully populated.
     */
    void swapBuffers() {
        std::lock_guard<std::mutex> lock(mtx_);
        std::swap(front_buffer_, back_buffer_);
    }

    /** @return Pointer to the front (read) buffer. */
    T* getFrontBuffer() {
        return front_buffer_.get();
    }

    /** @return Pointer to the back (write) buffer. */
    T* getBackBuffer() {
        return back_buffer_.get();
    }
};