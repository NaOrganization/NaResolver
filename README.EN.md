# Hi I2Hrame

"Excellent IL2cpp game plugin development framework."

------

<sub>(**This lib is a free but subject to agreement to use, if your company is using "Hi I2Hrame" please consider contacting me, thanks.**)</sub>

| [The Pitch](#The-Pitch)-[Usage](#Usage)-[How it works](#How-it-works)-[License](#License) |
| :----------------------------------------------------------: |



### The Pitch

Is a C++ language written by the lightweight Il2cpp game plug-in project framework

It has good compatibility, only need to change the IL2cpp version of the corresponding data structure can be compatible with different games

- Fast setup
- Good compatibility
- x64 & x86

### Usage

**(Pay attention!! You need to set the Types appropriate for the Unity version of your game)**

**Just introduce 'I2Hrame.h' into** your project and install the frame is ok

```cpp
if(I2Hrame->Setup())
    return false;
```

```cpp
// Get Class
Il2CppClass* _class = I2Hrame->GetClassEx("assembly", "namespace", "className");
_class = I2Hrame->GetClass("(assembly)namespace.className");

// Get Method
I2Hrame->GetMethod(_class, "returnType methodName(parameterType1, parameterType2)");
```

***(Note: Get method parameters with a space after ',')***

### How it works

Almost data and function is running in **GameAssembly.dll** this module.

SO We just get the IL2cpp give the public function is able to get **THE CLASS**, **THE METHODS**.

When you setup "Hi I2Hrame" I get the public method following.

```cpp
il2cpp_domain_get
il2cpp_domain_assembly_open
il2cpp_class_from_name
il2cpp_type_get_name
il2cpp_method_get_param_count
il2cpp_thread_attach
```

License
-------

Hi I2Hrame is licensed under the MIT License, see [LICENSE.txt](https://github.com/MidTerm-CN/I2Hrame/blob/main/LICENSE.txt) for more information.
