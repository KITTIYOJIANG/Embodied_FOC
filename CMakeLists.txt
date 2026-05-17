cmake_minimum_required(VERSION 3.16)
project(embodied_foc_core LANGUAGES CXX)

# ─── C++ Standard ────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Library: FOC Core (header-only helpers) ─────────────────────────────────
# All core templates (LoopUnroller, DoubleBuffer, Tensor) are header-only,
# so no compilation needed — simply link the include directory.
add_library(foc_core INTERFACE)
target_include_directories(foc_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ─── Demo: Double-Buffer (producer/consumer) ─────────────────────────────────
add_executable(demo_double_buffer
    src/demo_double_buffer.cpp
)
target_link_libraries(demo_double_buffer PRIVATE foc_core)

# ─── Demo: Compile-time Loop Unroller ────────────────────────────────────────
add_executable(demo_loop_unroller
    src/demo_loop_unroller.cpp
)
target_link_libraries(demo_loop_unroller PRIVATE foc_core)

# ─── Demo: constexpr Compile-time Computation ────────────────────────────────
add_executable(demo_constexpr
    src/demo_constexpr.cpp
)
target_link_libraries(demo_constexpr PRIVATE foc_core)

# ─── (Optional) ONNX Tensor Demo ─────────────────────────────────────────────
# Enables when ONNX Runtime is available:
#   find_package(onnxruntime REQUIRED)
#   add_executable(demo_tensor src/demo_tensor.cpp)
#   target_link_libraries(demo_tensor PRIVATE foc_core onnxruntime::onnxruntime)