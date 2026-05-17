import torch
import torch.nn as nn
import time

# --- 构建一个模拟模型（类 FOC 全连接网络）---
class FOCNet(nn.Module):
    def __init__(self):
        super().__init__()
        self.net = nn.Sequential(
            nn.Linear(512, 1024),
            nn.ReLU(),
            nn.Linear(1024, 2048),
            nn.ReLU(),
            nn.Linear(2048, 1024),
            nn.ReLU(),
            nn.Linear(1024, 6),   # 6 路 PWM 输出
        )
    def forward(self, x):
        return self.net(x)

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"使用设备: {device}")

model = FOCNet().to(device)
dummy_input = torch.randn(1, 512, device=device)

# 1. 极其重要：Warm-up (热身)
# GPU 刚启动时很慢（要分配显存、唤醒时钟），前 10 次推理的数据全扔掉！
print("开始热身...")
for _ in range(10):
    model(dummy_input)

# 2. 准备大厂级秒表
if device.type == "cuda":
    start_event = torch.cuda.Event(enable_timing=True)
    end_event = torch.cuda.Event(enable_timing=True)

# 3. 开始百次压测
iterations = 100
print("开始精确测速...")

if device.type == "cuda":
    start_event.record() # 按下秒表
    for _ in range(iterations):
        output = model(dummy_input)
    end_event.record() # 按下停止

    # 4. 强制 CPU 站在原地等 GPU 把菜全炒完！！！
    torch.cuda.synchronize()

    # 5. 算出平均耗时和 FPS
    total_time_ms = start_event.elapsed_time(end_event)
    avg_latency_ms = total_time_ms / iterations
    fps = 1000.0 / avg_latency_ms

    print(f"[原生 Baseline] 延迟 (Latency): {avg_latency_ms:.2f} ms")
    print(f"[原生 Baseline] 帧率 (FPS): {fps:.2f}")
else:
    # CPU fallback: 用 time.perf_counter
    t0 = time.perf_counter()
    for _ in range(iterations):
        output = model(dummy_input)
    t1 = time.perf_counter()
    total_time_ms = (t1 - t0) * 1000
    avg_latency_ms = total_time_ms / iterations
    fps = 1000.0 / avg_latency_ms
    print(f"[CPU Baseline] 延迟 (Latency): {avg_latency_ms:.2f} ms")
    print(f"[CPU Baseline] 帧率 (FPS): {fps:.2f}")
