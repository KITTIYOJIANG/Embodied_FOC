#include <iostream>
#include <thread>
#include <chrono>
#include "data_hub.h" // 里面放着你刚才看懂的 DoubleBuffer 类
#include "foc_math.h"

// 我们测试就用一个简单的 int 代表复杂的 Tensor 数据
DoubleBuffer<int> data_hub(1); 

// 1. 视觉线程：慢吞吞地写数据
void vision_thread_task() {
    for (int i = 1; i <= 5; ++i) {
        // 拿到后厨的盘子
        auto back = data_hub.getBackBuffer();
        
        // 模拟做菜很慢 (停顿 500 毫秒)
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        *back = i * 100; // 写入新数据
        
        std::cout << "[视觉] 第 " << i << " 帧画面处理完毕，准备 Swap！\n";
        data_hub.swapBuffers(); // 瞬间端到前台
    }
}

// 2. FOC 控制线程：极其疯狂地读数据
void foc_thread_task() {
    for (int i = 0; i < 15; ++i) {
        // 拿到前台的盘子（加了安全绳！）
        auto front = data_hub.getFrontBuffer();
        
        std::cout << "    [FOC] 读取电流，当前画面数据: " << *front << "\n";
        
        // 模拟高频控制 (停顿 200 毫秒)
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
    float currents[3] = {1.2f,-0.6f,-0.6f};
    float filter_gain = 0.9f;

    std::cout << "--- 启动具身智能多线程引擎 ---\n";
    
    // 强行展开一个 3 次的循环！
    // 这里的 3 是写在尖括号 <> 里的，意味着它是【编译期常量】
    // 编译器在看到这行代码时，会物理复制 3 行代码，把 for 循环彻底炸碎！
    LoopUnroller<3>::execute([&currents, filter_gain](size_t i) {
        currents[i] *= filter_gain;
        std::cout << "  [汇编展开] 成功无分支计算索引: " << i << " -> 结果: " << currents[i] << "\n";
    });

    // 初始化第一帧数据为 0
    *(data_hub.getBackBuffer()) = 0;
    data_hub.swapBuffers();

    // 真正启动物理多线程！
    std::thread t1(vision_thread_task);
    std::thread t2(foc_thread_task);

    // 等待两个线程跑完
    t1.join();
    t2.join();

    std::cout << "--- 引擎安全停机 ---\n";
    return 0;
}