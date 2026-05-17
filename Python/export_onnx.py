import torch
import torchvision.models as models
import os

print("🔄 正在 Python 环境中初始化 ResNet-18 结构...")
# 1. 纯结构加载，不需要权重
model = models.resnet18(weights=None).eval()

# 2. 核心：准备一个虚拟输入 (Dummy Input) 
# 形状固定为: [Batch_Size, Channels, Height, Width] -> [1, 3, 224, 224]
dummy_input = torch.randn(1, 3, 224, 224)

onnx_filename = "resnet18.onnx"
print(f"📐 虚拟输入 Tensor 形状: {dummy_input.shape}")
print(f"📦 开始导出为跨平台 ONNX 蓝图...")

# 3. 执行导出
torch.onnx.export(
    model, 
    dummy_input, 
    onnx_filename, 
    verbose=False, 
    input_names=['input'], 
    output_names=['output'], 
    opset_version=12
)

if os.path.exists(onnx_filename):
    print(f"✅ 成功！ONNX 蓝图文件已生成在当前目录下: {os.path.abspath(onnx_filename)}")
else:
    print("❌ 导出失败，请检查 PyTorch 环境。")