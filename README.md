# Hi I2Hrame

"优秀的IL2cpp游戏插件开发框架."

------

| [特点](#特点)-[如何使用](#如何使用)-[实现原理](#实现原理) |
| :-------------------------------------------------------: |
|                  [English](README.EN.md)                  |



### 特点

"Hi I2Hrame"是一个由C++语言编写的轻量化的Il2cpp游戏插件项目框架

"Hi I2Harme"有较好的兼容性，只需要更改IL2cpp版本相对应的数据结构即可兼容到不同的游戏中

- 快速初始化
- 较好的兼容性
- 支持x64与x86

### 如何使用

***(注意！！你需要设置合适游戏Unity版本的Types)***

只需要将一些文件**引入**并**安装**一遍框架

```cpp
// 引入例子:
#include <I2Hrame/Types/27.2-2021.1.0-2021.1.99+.h>
#include <I2Hrame/I2Hrame.h>
// 注意: 版本文件必须要早于核心文件引入
// 我要怎么选择版本文件呢？
// (27.2-2021.1.0-2021.1.99+) 27.2是IL2cpp版本, 2021.1.0-2021.1.99+是unity版本
```

```cpp
// 安装例子:
// 参数: I2HConfig
// struct I2HConfig
// {
// 	bool disableGC = false;
//	bool enableLogger = false;
//
//	struct LoggerConfig
//	{
//		void(*fatal)(std::string, ...);
//		void(*info)(std::string, ...);
//		void(*debug)(std::string, ...);
//		void(*error)(std::string, ...);
//	} logger;
//};
// 传入你需要的配置
if(I2Hrame->Setup(config))
    return false;
```

```cpp
// 获取类
Il2CppClass* _class = I2Hrame->GetClassEx("assembly", "namespace", "className");
_class = I2Hrame->GetClass("(assembly)namespace.className");

// 获取方法
I2Hrame->GetMethod(_class, "returnType methodName(parameterType1, parameterType2)");
```

***(注意: 获取方法时参数之间的 ',' 后面有一个空格)***

### 实现原理

在IL2cpp中基本的所有运行数据都在**GameAssembly.dll**模块

在**GameAssembly.dll**里通过IL2cpp给予的Api去获取到**类，方法，程序集等**信息.

在安装框架时我们获取了几个Api分别是

```cpp
il2cpp_domain_get
il2cpp_domain_assembly_open
il2cpp_class_from_name
il2cpp_type_get_name
il2cpp_method_get_param_count
il2cpp_thread_attach
```

协议
-------

"Hi I2Hrame"使用MIT协议, 详细见 [LICENSE.txt](https://github.com/MidTerm-CN/I2Hrame/blob/main/LICENSE.txt).
