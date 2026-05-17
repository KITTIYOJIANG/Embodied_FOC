#pragma once
#include <memory>
#include <mutex>

template<typename T>
class DoubleBuffer {
    private:
        std::unique_ptr<T[]> frontBuffer; //读 顾客一直在看新的招牌菜
        std::unique_ptr<T[]> backBuffer;    //写 厨师在后厨切菜，颠勺动作很大很慢
        std::mutex mutex;   //极细颗粒度 餐厅大堂经理

    public:
        DoubleBuffer(size_t size)
            : frontBuffer(std::make_unique<T[]>(size))
            , backBuffer(std::make_unique<T[]>(size))
        {}

        // 瞬间交接
        void swapBuffers() {
            std::lock_guard<std::mutex> lock(mutex);    //经理喊停所有人不许动
            std::swap(frontBuffer, backBuffer); //这里瞬间下后厨把菜端到前台，把前台的空盘子收到后厨
        }

        T* getFrontBuffer() {
            return frontBuffer.get();
        }
        
        // 后厨做菜
        T* getBackBuffer() {
            return backBuffer.get();
        }
};
