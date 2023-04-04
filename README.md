Hi I2Hrame

"最好的IL2cppHack框架."

------

<sub>(**这个库是一个自由但在遵守协议的情况下使用，如果您的公司正在使用"Hi I2Hrame"，请考虑联系我，谢谢.**)</sub>

| [特点](#特点)-[如何使用](#如何使用)-[实现原理](#实现原理) |
| :-------------------------------------------------------: |



### 特点

"Hi I2Hrame"是一个由C++语言编写的轻量化的Il2cppHack项目框架

"Hi I2Harme"有较好的兼容性，只需要更改IL2cpp版本相对应的数据结构即可兼容到不同的游戏中

- 快速初始化
- 较好的兼容性

### 如何使用

***(注意！！你必须自行添加IL2cppType.h这个文件，你可以使用[这个工具](https://github.com/djkaty/Il2CppInspector)去获取)***

**只需要将 'I2Hrame.h' 引入**并你安装一遍框架

```cpp
if(I2Hrame.Setup())
    return false;
```

***(注意: 需要安装一遍I2Harme，否则无法使用其功能！)***

```cpp
// 获取类
Il2CppClass* _class = I2Hrame.GetClassEx("assembly", "namespace", "className");
_class = I2Hrame.GetClass("(assembly)namespace.className");

// 获取方法
I2Hrame.GetMethod(_class, "returnType methodName(parameterType1, parameterType2)");
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

使用"Hi I2Hrame"请遵守其使用协议, 详细见 [LICENSE.txt](LICENSE.txt).
