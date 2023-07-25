# NaResolver

"Lightweight UnityGame plugin development framework，Give you a excellent experience."

| [Introduction](#Introduction) - [The Pitch](#The-Pitch) - [Usage](#Usage) - [How it works](#How-it-works) - [License](#License) |
| :----------------------------------------------------------: |

### Introduction

**NaResolver** is a lightweight development framework for plugin of UnityGame from the **Perfect Team**'s sub project **"NaLibrary"**

**NaLibrary** projects, including **NaResolver**, are all open source and use **MIT** as a license.

### The Pitch

**NaResolver** is a lightweight development  framework for plugin of UnityGame written in C++ language

**NaResolver** has excellent compatibility

- Fast setup
- Excellent compatibility
- Support x64 and x86
- Support Mono by other branch
- Support Il2Cpp29 version 

### Usage

#### How to setup？

```cpp
// example (with Il2CppTypes):
#include <NaResolver/Types/27.2-2021.1.0-2021.1.99+.h>
#include <NaResolver/NaResolver.h>
// Attention: The type file must to include earlier than core file.

// example2 (without Il2CppTypes):
#define __NARESOLVER_ONLY_API
#include <NaResolver/NaResolver.h>
```

`27.2`  : Il2Cpp version

`2021.1.0-2021.1.99+` : UnityEngine version

```cpp
NaResolver::Config config;
if(Il2CppResolver->Setup(config))
    return false;
```

#### How to get class, method and field?

```cpp
// Get class
Il2CppClass* _class = Il2CppResolver->GetClassEx("assembly", "namespace", "className");
_class = Il2CppResolver->GetClass("(assembly)namespace.className");

// Get method
Il2CppResolver->GetMethod(_class, "returnType methodName(parameterType1, parameterType2)");

// Get field
Il2CppResolver->GetField(_class, "fieldName");
```

***(Attention: Get method parameters with a space after ',')***

### How it works

Access to information is achieved by using the **UnityEngine** engine that exposes the **API** at **Runtime**.

**Il2Cpp** Exposed **API** from `GameAssembly.dll`.

**Mono** Exposed **API** from `mono-2.0-bdwgc.dll`.

## License

**NaResolver** is licensed under the MIT License, see [LICENSE.txt](LICENSE.txt) for more information.
