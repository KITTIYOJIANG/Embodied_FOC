/**
 * @file    demo_double_buffer.cpp
 * @brief   Demonstrates thread-safe double-buffer between vision/perception
 *          (producer) and FOC control (consumer) threads.
 *
 * Build:
 *   cmake -B build && cmake --build build
 *   ./build/demo_double_buffer
 */

#include <iostream>
#include <thread>
#include <chrono>
#include "double_buffer.h"

// Global double-buffer with a single int representing complex perception data
DoubleBuffer<int> data_hub(1);

/**
 * @brief  Producer thread: simulates a vision pipeline running at ~2 Hz.
 *
 * Writes a new frame number into the back-buffer, then atomically
 * swaps buffers to make it visible to the consumer.
 */
void vision_thread_task() {
    for (int i = 1; i <= 5; ++i) {
        auto back = data_hub.getBackBuffer();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        *back = i * 100;
        std::cout << "[Vision] Frame " << i << " processed, swapping buffers.\n";
        data_hub.swapBuffers();
    }
}

/**
 * @brief  Consumer thread: simulates a FOC control loop running at ~5 kHz.
 *
 * Reads the latest perception data from the front-buffer and
 * applies it to the next control cycle.
 */
void foc_thread_task() {
    for (int i = 0; i < 15; ++i) {
        auto front = data_hub.getFrontBuffer();
        std::cout << "    [FOC] Control cycle " << i
                  << " — perception data: " << *front << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
    std::cout << "--- Embodied_FOC_Core: Double-Buffer Demo ---\n";

    // Initialize back-buffer with zero
    *(data_hub.getBackBuffer()) = 0;
    data_hub.swapBuffers();

    // Launch producer and consumer threads
    std::thread t1(vision_thread_task);
    std::thread t2(foc_thread_task);

    t1.join();
    t2.join();

    std::cout << "--- Demo complete, all threads joined ---\n";
    return 0;
}