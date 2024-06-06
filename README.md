#  NaResolver V2

"轻量化的Unity游戏插件开发框架，给你带来极佳的体验."

------

[English Readme Here](README.en.md)

### 介绍

**NaResolver**是为插件开发研发的一款轻量化开发框架. 

### 未来

- [ ] 编写Wiki

### 特点

- 标准库字符串与游戏之间无痛转换

- 轻量化的开发设计

- 简单的安装

- 强劲的兼容性
  
- 同时支持Mono与Il2Cpp

- 拥有TestEngine，让错误快速显现

- 简单易用

### 如何使用

#### 初始化

```cpp
#define NA_RESOLVER_STRING_XOR _xor_ // 如果你需要使用xor
#define NA_RESOLVER_TEST_ENGINE // 如果你需要使用TestEngine
#include <NaResolver/NaResolver.h>

...main()
{
	if (naResolverInstance.Setup())
        printf("Setup successfully!");
}
```

#### (语法糖) 获取类、方法、字段
```cpp
#define NA_RESOLVER_STRING_XOR _xor_ // 如果你需要使用xor
#define NA_RESOLVER_TEST_ENGINE // 如果你需要使用TestEngine
#include <NaResolver/NaResolver.h>

...main()
{
	if (naResolverInstance.Setup())
        printf("Setup successfully!");
}
```

#### (底层) 获取类、方法、字段

```cpp
// 类声明
class NormalClass
{
public:
// 获取类
// CLASS(assemblyName, namespaceName, className);
CLASS(FakeAssembly, FakeNamespace, NormalClass);

// 嵌套类声明
class NestedClass
{
public:
	// 获取嵌套类
	// NESTED_CLASS(declaringClassName, nestedClassName);
	NESTED_CLASS(NormalClass, NestedClass);
};

// 静态成员字段声明
// STATIC_FIELD(type, name);
// STATIC_BACKING_FIELD(type, name); // BackingField
STATIC_FIELD(int, fakeStaticField);

// 实例成员字段声明
// FIELD(type, name);
// BACKING_FIELD(type, name);
FIELD(int, fakeField);

// 方法声明
// METHOD(parameterCount, returnTypeName, methodName, ...methodParameterTypes);
METHOD(1, "System.Int32", fakeMethod, "System.String");
int fakeMethod(std::string arg1)
{
	return Template::MethodInvoker<void*>(__fakeMethod_1_Method_Info__.GetMethodAddresss())(VmGeneralType::String(args1));
}
// 注意：这里展示了一个比较特殊的类型(String)的传递
};
```
#### 方法的调用、字段的读写

**方法的调用**
```cpp
// 1. 获取方法
auto methodInstance = naResolverInstance.GetMethod(klass, "returnType", "methodName", {"parameterType1", "parameterType2"});
// 2. 获取方法地址
auto methodInvokeAddress = methodInstance.method.GetInvokeAddress();
// 3. 函数指针
// ...
```

**字段的读写**
```cpp
// 1. 获取字段
auto fieldInfo = klass.klass.GetField("name");
// 2. 读取
int value;
fieldInfo.GetValue(object, &value);
// 3. 写入
fieldInfo.SetValue(object, &value);
```

协议
-------

**NaResolver**使用GPL3.0协议, 详细见 [LICENSE.txt](LICENSE.txt).
