# UniResolver

UniResolver 是一个面向 Windows IL2CPP 运行时的 C++20 仅头文件解析与调用层。它负责绑定 `GameAssembly.dll` 导出的 IL2CPP API，并为 UniDumper 生成的 SDK 提供类、字段、方法及闭合泛型的运行时解析能力。

[English](README.en.md) · [宏与用法 Wiki](Wiki.md) · [示例工程](example)

## 特点

- 直接使用 UniDumper 生成的 C++ SDK 声明。
- 按 assembly、namespace、类型名或 metadata token 延迟解析并缓存结果。
- 通过 `il2cpp_runtime_invoke` 调用方法，支持实例方法、虚方法和托管异常。
- `std::string` 与 IL2CPP `String` 自动转换。
- 支持静态字段、嵌套类型、值类型、名称重定位和闭合泛型。

## 环境要求

- Windows 与 MSVC。
- C++20（示例工程使用 Visual Studio 2022 / `v143`）。
- 目标程序使用 IL2CPP，并已加载 `GameAssembly.dll`。

## 快速开始

1. 将 `UniResolver.h`、`VmApi.h` 和 `KrOr.h` 放入项目的 include 目录。
2. 开启 C++20，并包含 UniResolver 与 UniDumper 生成的 SDK。
3. 在调用任何 SDK 包装前执行 `UniResolver.Setup()`。

```cpp
#include "UniResolver.h"
#include "UniDumperSdk.h"

void PrintVersions()
{
	if (!UniResolver.Setup())
	{
		return;
	}

	const std::string unityVersion =
		UnityEngine::Application::get_unityVersion();
	const std::string gameVersion =
		UnityEngine::Application::get_version();

	UniResolver.Destroy();
}
```

完整示例是一个 DLL 工程：[main.cpp](example/main.cpp) 展示初始化和调用流程，[UniDumperSdk.h](example/UniDumperSdk.h) 则保留一小段 UniDumper 生成的 `System::Object`、`UnityEngine::Object` 与 `UnityEngine::Application` SDK。metadata token 会随目标版本变化，实际项目应使用对应版本重新生成的 SDK。

所有公开声明宏、参数含义、泛型定位器、字段读写、方法调用与名称映射示例见 [Wiki](Wiki.md)。

## License

[GPL-3.0](LICENSE)
