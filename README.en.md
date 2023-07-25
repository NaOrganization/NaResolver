# NaResolver

"Lightweight UnityGame plugin development framework，Give you a excellent experience."

| [Introduction](#Introduction) - [The Pitch](#The-Pitch) - [Usage](#Usage) - [How it works](#How-it-works) - [License](#License) |
| :----------------------------------------------------------: |

### Introduction

**NaResolver** is a lightweight development framework for plugin of UnityGame from the **Perfect Team**'s sub project **"NaLibrary"**

**NaLibrary** projects, including **NaResolver**, are all open source and use **MIT** as a license.

### The Pitch

"NaResolver"是一个由C++语言编写的轻量化的UnityEngine游戏插件项目框架

"NaResolver"有强劲兼容性

- 快速初始化
- 较好的兼容性
- 支持x64与x86
- 同时拥有 Mono 适配版本
- 支持 Il2Cpp29+ 版本

### Usage

#### 如何安装框架呢？

```cpp
// 引入例子 (需要使用Il2CppType时):
#include <NaResolver/Types/27.2-2021.1.0-2021.1.99+.h>
#include <NaResolver/NaResolver.h>
// 注意: 版本文件必须要早于核心文件引入

// 引入例子 (不需要使用Il2CppType时):
#define __NARESOLVER_ONLY_API
#include <NaResolver/NaResolver.h>
```

`27.2`  : IL2cpp版本

`2021.1.0-2021.1.99+` : UnityEngine版本

```cpp
NaResolver::Config config;
if(Il2CppResolver->Setup(config))
    return false;
```

#### 要如何获取类和方法呢？

```cpp
// 获取类
Il2CppClass* _class = Il2CppResolver->GetClassEx("assembly", "namespace", "className");
_class = Il2CppResolver->GetClass("(assembly)namespace.className");

// 获取方法
Il2CppResolver->GetMethod(_class, "returnType methodName(parameterType1, parameterType2)");

// 获取字段
Il2CppResolver->GetField(_class, "fieldName");
```

***(注意: 获取方法时参数之间的 ',' 后面有一个空格)***

### How it works

通过使用**UnityEngine**引擎在**Runtime**时会暴露**API**的特性来实现获取信息.

**Il2Cpp**暴露的**API**来源于`GameAssembly.dll`.

**Mono**暴露的**API**来源于`mono-2.0-bdwgc.dll`.

## 协议

**NaResolver**使用MIT协议, 详细见 [LICENSE.txt](https://github.com/MidTerm-CN/I2Hrame/blob/main/LICENSE.txt).