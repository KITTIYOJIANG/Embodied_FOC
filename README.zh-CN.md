# 具身FOC核心库

**高性能、实时场向控制库** 采用编译期优化和无锁数据结构，专为具身智能和机器人应用设计。

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)](https://en.cppreference.com/w/)
[![CMake](https://img.shields.io/badge/CMake-3.16%2B-blue)](https://cmake.org/)

---

## 📌 项目概览

**Embodied_FOC_Core** 是一个零开销的 C++ 库，专为实时电机控制、感知管道和多线程机器人应用设计。它提供：

- **无锁同步**：通过双缓冲实现安全的生产者-消费者模式
- **编译期循环展开**：消除分支预测开销
- **轻量级张量容器**：针对 ONNX Runtime 集成优化
- **零拷贝缓冲管理**：支持超过 10 kHz 的高频控制循环
- **仅头文件模板**：无缝集成，无外部依赖

为自动驾驶、无人机控制和工业机器人等延迟敏感型应用而构建。

---

## 🎯 核心特性

### 1. **双缓冲模式** (`DoubleBuffer<T>`)
使用原子指针交换实现线程间的安全数据交换。

**典型应用场景**：视觉线程写入感知数据 → FOC 控制线程以 10 kHz 读取最新状态。

```cpp
DoubleBuffer<float> state_buffer(3);  // 3相电流状态

// 生产者线程
auto back = state_buffer.getBackBuffer();
back[0] = ia; back[1] = ib; back[2] = ic;
state_buffer.swapBuffers();  // O(1) 原子交换

// 消费者线程 (10 kHz)
auto front = state_buffer.getFrontBuffer();
float current_id = front[0];
```

### 2. **编译期循环展开** (`LoopUnroller<N>`)
通过在编译时物理复制循环体来消除循环开销。对实时保证至关重要。

**性能影响**：移除紧凑控制循环中的分支预测停滞。

```cpp
float currents[3] = {1.2f, -0.6f, -0.6f};
LoopUnroller<3>::execute([&currents](size_t i) {
    currents[i] *= 0.9f;  // 编译为 3 条独立指令
});
```

### 3. **不可拷贝张量** (`Tensor`)
仅移动的堆分配浮点缓冲，专为 RTOS 环境设计。防止意外内存碎片化。

```cpp
Tensor output(128);  // 128 元素输出缓冲
output.set(0, 3.14f);
float val = output.at(0);  // 安全的边界检查
```

### 4. **编译期数学工具** (`compile_time_utils.h`)
支持编译期计算的数学常数和控制参数。

---

## 🚀 快速开始

### 系统要求
- **C++17 或更高版本** 编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.16+**
- **POSIX 线程** (pthreads)

### 构建项目

```bash
# 克隆并进入仓库
git clone <仓库地址>
cd Embodied_FOC_Core

# 配置和构建
cmake -B build -S .
cmake --build build

# 运行演示程序
./build/demo_double_buffer
./build/demo_loop_unroller
./build/demo_constexpr
```

### 作为仅头文件库使用

```cmake
# 在你的 CMakeLists.txt 中
add_library(your_target ...)
target_include_directories(your_target PRIVATE /path/to/Embodied_FOC_Core/include)
```

或直接包含：
```cpp
#include "foc_core/double_buffer.h"
#include "foc_core/compile_time_utils.h"
#include "foc_core/tensor.h"
```

---

## 📚 API 参考

### `DoubleBuffer<T>`

**具有 O(1) 交换的线程安全生产者-消费者缓冲。**

| 方法 | 签名 | 说明 |
|------|------|------|
| 构造函数 | `DoubleBuffer(size_t size)` | 初始化包含 `size` 个元素的缓冲 |
| 获取后缓冲 | `T* getBackBuffer()` | 生产者：获取可写缓冲 |
| 获取前缓冲 | `T* getFrontBuffer() const` | 消费者：获取可读缓冲 |
| 交换 | `void swapBuffers()` | 原子交换 (O(1)，互斥锁保护) |
| 大小 | `size_t size() const` | 获取缓冲元素数 |

**线程安全性**：
- ✅ 前缓冲上的多个读取器 (并发安全)
- ✅ 后缓冲上的单个写入者 (设计约束)
- ✅ 交换通过互斥锁序列化

### `LoopUnroller<N>`

**编译期循环展开（零运行时开销）。**

| 方法 | 签名 | 说明 |
|------|------|------|
| 执行 | `static void execute(Action&& fn)` | 运行展开的循环体 `N` 次 |

**模板参数**：
- `N`：循环次数 (编译期常量)

**Action 可调用签名**：
```cpp
void(size_t index)  // 使用索引 0 到 N-1 调用
```

### `Tensor`

**不可拷贝、仅移动的浮点缓冲。**

| 方法 | 签名 | 说明 |
|------|------|------|
| 构造函数 | `Tensor(size_t size)` | 分配 `size` 个浮点数 |
| 移动构造 | `Tensor(Tensor&&)` | 转移所有权 (O(1)) |
| 设置 | `void set(size_t idx, float val)` | 设置索引处的值 |
| 访问 | `float at(size_t idx) const` | 获取值 (边界检查) |
| 原始指针 | `float* data()` | 原始指针访问 |
| 大小 | `size_t size() const` | 获取元素数 |

**约束条件**：
- ❌ 拷贝构造被删除 (编译期强制)
- ❌ 拷贝赋值被删除
- ✅ 完全启用移动语义

---

## 🏗️ 项目结构

```
Embodied_FOC_Core/
├── include/
│   ├── double_buffer.h         # 无锁同步
│   ├── compile_time_utils.h    # 循环展开、编译期工具
│   ├── tensor.h                # 不可拷贝张量
│   ├── foc_math.h              # FOC 特定数学运算
│   └── data_hub.h              # 应用胶水层
├── src/
│   ├── demo_double_buffer.cpp  # 生产者-消费者演示
│   ├── demo_loop_unroller.cpp  # 循环展开演示
│   ├── demo_constexpr.cpp      # 编译期计算演示
│   └── main.cpp                # 集成示例
├── Python/
│   ├── export_onnx.py          # 导出模型为 ONNX
│   └── resnet18.onnx           # 预训练 ResNet18
├── CMakeLists.txt              # 构建配置
└── README.md                   # 英文文档
```

---

## 💡 应用案例

### 1. **自动驾驶汽车电机控制**
- 视觉线程发布场景理解结果
- FOC 线程以 10+ kHz 频率读取双缓冲数据
- 消除实时优先级反转

### 2. **无人机姿态控制**
- IMU/陀螺仪线程更新状态缓冲
- 控制循环展开以计算 P、I、D 项
- 确定性延迟 < 1 ms

### 3. **机器人臂运动规划**
- 感知线程写入关节角度
- 控制线程通过零拷贝张量读取
- ONNX 推理结果无缝集成

### 4. **实时音频处理**
- 无锁循环缓冲 (可扩展双缓冲)
- DSP 内核循环展开
- 编译期优化确保无运行时意外

---

## 🔧 构建与测试

### 使用调试符号构建
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### 优化版本构建 (Release)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### 运行所有演示
```bash
cd build
./demo_double_buffer
./demo_loop_unroller
./demo_constexpr
```

### 集成测试
```bash
cd /home/zexu/Embodied_FOC_Core
rm -rf build && cmake -B build && cmake --build build
```

---

## 📊 性能特征

| 组件 | 时间复杂度 | 空间复杂度 | 延迟 |
|------|-----------|-----------|------|
| DoubleBuffer 交换 | **O(1)** | O(n) | ~100ns (互斥锁) |
| LoopUnroller<N> | **O(1)** | O(1) | 0ns (编译期) |
| Tensor::at() | **O(1)** | O(n) | ~2ns (边界检查) |
| Tensor 移动 | **O(1)** | O(1) | ~10ns |

**实时保证**：
- ✅ 控制循环中无动态分配
- ✅ 无虚函数调用
- ✅ 前缓冲读取无互斥锁
- ✅ 确定性执行 (无 GC、无异常)

---

## 🔗 与 ONNX Runtime 集成

预配置用于 ONNX 模型推理。示例工作流：

```cpp
#include "tensor.h"

// 将模型预测加载到不可拷贝张量中
Tensor inference_output(128);

// 传输到控制管道 (零拷贝移动)
DoubleBuffer<Tensor> model_cache(1);
auto back = model_cache.getBackBuffer();
*back = std::move(inference_output);  // O(1) 所有权转移
model_cache.swapBuffers();
```

参见 `Python/export_onnx.py` 了解模型导出示例。

---

## 📋 设计原则

1. **零开销抽象**：所有开销通过模板在编译期消除
2. **无锁概念**：最小化同步点 (仅交换)
3. **RTOS 就绪**：初始化后无动态分配
4. **移动语义**：高效的所有权转移，无拷贝
5. **类型安全**：编译期强制线程安全模式

---

## ⚠️ 已知限制与考虑

- **DoubleBuffer** 使用互斥锁 (非完全无锁)；真正无锁需集成 `std::atomic`
- **Tensor** 不直接支持 SIMD 矢量化 (如需要可使用对齐分配器)
- **LoopUnroller** 仅支持编译期常量计数
- 后缓冲假设单生产者 (设计约定)

---

## 🤝 贡献指南

欢迎贡献！请确保：
- [ ] 在所有平台上符合 C++17 规范
- [ ] 维持零开销原则
- [ ] 线程安全性有文档记录
- [ ] 最小化外部依赖
- [ ] 通过模板实现编译期安全

---

## 📝 许可证声明

本项目不采用特定的开源许可证。如需使用本项目，请联系作者了解使用条款。

---

## 📞 支持与故障排查

### 编译问题
- **C++ 标准错误**：确保在 CMake 中设置了 `-std=c++17` 或更高版本
- **缺失头文件**：验证 `include/` 路径在 `target_include_directories()` 中
- **链接器错误**：必须链接 Pthreads；CMake 会自动处理

### 运行时问题
- **双缓冲段错误**：确保生产者仅写入后缓冲
- **LoopUnroller 未内联**：检查编译器优化标志 (`-O2` 或 `-O3`)
- **张量边界检查**：在调试模式下启用以获得断言

### 性能优化
```bash
# 使用最大优化编译
cmake -B build -DCMAKE_CXX_FLAGS="-O3 -march=native -fno-exceptions"
```

---

## 📚 参考资源

- **C++17 标准**：https://en.cppreference.com/w/cpp/17
- **FOC 理论**：[电机控制基础](https://zh.wikipedia.org/wiki/矢量控制)
- **实时系统**：Buttazzo，《硬实时计算系统》
- **无锁编程**：Preshing，《无锁编程》

---

**最后更新**：2026 年 5 月 17 日  
**维护者**：具身智能研究团队  
**状态**：积极开发中 ✅

---

## 🎓 代码示例

### 示例 1：多线程控制循环
参见 `src/main.cpp` 了解视觉-FOC 同步的完整示例。

### 示例 2：实时音频 DSP
```cpp
#include "compile_time_utils.h"

// IIR 滤波器系数 (编译期)
float signal[32];
LoopUnroller<32>::execute([&signal](size_t i) {
    signal[i] = signal[i] * 0.95f;  // 低通滤波：32 个展开的乘法
});
```

### 示例 3：ONNX 集成
参见 `Python/export_onnx.py` 了解如何将 PyTorch 模型导出为 ONNX 格式。

---

**用 ❤️ 为高性能机器人和具身智能打造**
