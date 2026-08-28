# UniResolver Wiki：宏与具体用法

本文覆盖 `UniResolver.h` 提供给 UniDumper SDK 生成器和 SDK 使用者的全部 24 个公开声明宏。`H_NARESOLVER`、临时的 `TEXT`、`VmApi.h` 的 `VM_API_SYMBOL`，以及 `KrOr.h` 中的实现宏属于头文件内部机制，不是 SDK 声明 API。

## 1. 使用约定

### 1.1 初始化与文件顺序

在使用任何生成包装前初始化全局上下文：

```cpp
if (!UniResolver.Setup())
{
	return;
}
```

推荐的 SDK 声明顺序是：普通类型与值类型、泛型定义与模板、闭合泛型特化、业务调用代码。`CLOSED_GENERIC_CLASS_INFO` 和 `CLOSED_GENERIC_METHOD_INFO` 必须位于相关类型声明之后，并出现在对应模板组合第一次实例化之前。

除两个 `CLOSED_GENERIC_*` 宏外，其余宏通常写在类的 `public` 区域内。所有宏调用末尾都要加分号。

### 1.2 四类名称与类型

- `assemblyName`：metadata 中的程序集名，例如 `"Assembly-CSharp"` 或 `"UnityEngine.CoreModule"`。
- `namespaceName`：metadata 命名空间；全局命名空间使用 `""`。
- `className`：合法的 C++ 类型标识符。
- `metadataName`：metadata 中的真实类型或成员名，可包含反引号等 C++ 标识符不允许的字符。
- `storageType`：字段布局中实际存放的 C++ 类型。引用类型通常是 `T*`，值类型通常是 `T`，基础类型可以是 `std::int32_t` 等。
- `token`：方法的 metadata token，例如 `0x0600006D`；它不是 RVA，也不是函数地址。
- `locator`：由 UniDumper 给出的运行时定位器，使用 `VmGeneralType::RelativePointer<T>::FromRva` 或 `FromPointerRva`。

### 1.3 宏索引

| 宏 | 用途 |
| --- | --- |
| `MANAGED_TYPE_NAMED` | 显式声明普通托管类型及其存储类型 |
| `CLASS_NAMED` | metadata 名与 C++ 名不同的引用类型 |
| `CLASS` | metadata 名与 C++ 名相同的引用类型 |
| `VALUE_CLASS_NAMED` | metadata 名与 C++ 名不同的内联值类型 |
| `VALUE_CLASS` | metadata 名与 C++ 名相同的内联值类型 |
| `VALUE_CLASS_STORAGE_NAMED` | 显式 metadata 名和底层存储的值类型 |
| `VALUE_CLASS_STORAGE` | 显式底层存储的值类型 |
| `GENERIC_CLASS_DEFINITION` | 声明开放泛型的 metadata 身份 |
| `GENERIC_CLASS` | 声明闭合泛型引用类型模板 |
| `GENERIC_VALUE_CLASS` | 声明闭合泛型值类型模板 |
| `CLOSED_GENERIC_CLASS_INFO` | 注册一个真实存在的闭合泛型类定位器 |
| `CLOSED_GENERIC_METHOD_INFO` | 注册一个真实存在的闭合泛型方法定位器 |
| `NESTED_MANAGED_TYPE_NAMED` | 显式声明嵌套类型及其存储类型 |
| `NESTED_CLASS_NAMED` | metadata 名与 C++ 名不同的嵌套引用类型 |
| `NESTED_CLASS` | metadata 名与 C++ 名相同的嵌套引用类型 |
| `NESTED_VALUE_CLASS_NAMED` | metadata 名与 C++ 名不同的嵌套值类型 |
| `NESTED_VALUE_CLASS` | metadata 名与 C++ 名相同的嵌套值类型 |
| `NESTED_VALUE_CLASS_STORAGE_NAMED` | 显式 metadata 名和存储的嵌套值类型 |
| `NESTED_VALUE_CLASS_STORAGE` | 显式存储的嵌套值类型 |
| `STATIC_FIELD_NAMED` | metadata 名与 C++ 名不同的静态字段代理 |
| `STATIC_FIELD` | 普通静态字段代理 |
| `STATIC_BACKING_FIELD_NAMED` | 显式属性 metadata 名的静态 backing field |
| `STATIC_BACKING_FIELD` | 属性名与 C++ 名相同的静态 backing field |
| `METHOD_INFO` | 按 metadata token 声明方法信息缓存 |

## 2. 普通类型宏

### 2.1 `MANAGED_TYPE_NAMED`

```cpp
MANAGED_TYPE_NAMED(assemblyName, namespaceName, className, metadataName, storageType);
```

这是普通类型宏的底层形式，同时指定 metadata 身份和字段存储类型。它会生成 `__This_Class_Type__`、`ManagedStorageType` 和 `ThisClassInfo`。只有 UniDumper 生成器或需要特殊布局时才应直接使用。

```cpp
namespace System
{
	class Boolean
	{
	public:
		MANAGED_TYPE_NAMED("mscorlib", "System", Boolean, "Boolean", bool);
	};
}
```

### 2.2 `CLASS_NAMED`

```cpp
CLASS_NAMED(assemblyName, namespaceName, className, metadataName);
```

声明引用类型，存储类型固定为 `className*`。当 metadata 名不能直接作为 C++ 标识符，或生成器改写了 C++ 名时使用。

```cpp
class GameObjectType
{
public:
	CLASS_NAMED(
		"UnityEngine.CoreModule", "UnityEngine", GameObjectType, "GameObject");
};
```

### 2.3 `CLASS`

```cpp
CLASS(assemblyName, namespaceName, className);
```

最常用的引用类型宏。metadata 名自动取 `#className`，字段中的该托管类型由 `className*` 表示。

```cpp
namespace UnityEngine
{
	class GameObject
	{
	public:
		CLASS("UnityEngine.CoreModule", "UnityEngine", GameObject);
	};
}
```

类信息可通过 `GameObject::ThisClassInfo.Instance()` 取得，返回 `UniResolverContext::Class`。

### 2.4 `VALUE_CLASS_NAMED`

```cpp
VALUE_CLASS_NAMED(assemblyName, namespaceName, className, metadataName);
```

声明 metadata 名与 C++ 名不同的内联值类型，存储类型为 `className` 自身。

```cpp
class Vector3Value
{
public:
	VALUE_CLASS_NAMED(
		"UnityEngine.CoreModule", "UnityEngine", Vector3Value, "Vector3");

	float x;
	float y;
	float z;
};
```

### 2.5 `VALUE_CLASS`

```cpp
VALUE_CLASS(assemblyName, namespaceName, className);
```

声明名称相同的内联值类型。结构体字段会直接保存该 C++ 类型，不保存指针。

```cpp
namespace UnityEngine
{
	class Vector3
	{
	public:
		VALUE_CLASS("UnityEngine.CoreModule", "UnityEngine", Vector3);

		float x;
		float y;
		float z;
	};
}
```

### 2.6 `VALUE_CLASS_STORAGE_NAMED`

```cpp
VALUE_CLASS_STORAGE_NAMED(
	assemblyName, namespaceName, className, metadataName, storageType);
```

同时显式指定 metadata 名和底层存储。基础类型、枚举包装或 C++ 包装类型名称被改写时使用。

```cpp
class Int32Value
{
public:
	VALUE_CLASS_STORAGE_NAMED(
		"mscorlib", "System", Int32Value, "Int32", std::int32_t);
};
```

此时 `Template::ManagedStorage<Int32Value>` 是 `std::int32_t`。

### 2.7 `VALUE_CLASS_STORAGE`

```cpp
VALUE_CLASS_STORAGE(assemblyName, namespaceName, className, storageType);
```

metadata 名自动取 `#className`，底层存储由 `storageType` 指定。

```cpp
namespace System
{
	class Int32
	{
	public:
		VALUE_CLASS_STORAGE("mscorlib", "System", Int32, std::int32_t);
	};
}
```

## 3. 泛型宏

以下示例假定已声明 `System::Int32` 和 `UnityEngine::GameObject` 等托管类型身份。泛型参数必须传身份类型，例如 `UnityEngine::GameObject`，不能传 `UnityEngine::GameObject*` 或 `std::int32_t`。

### 3.1 `GENERIC_CLASS_DEFINITION`

```cpp
GENERIC_CLASS_DEFINITION(assemblyName, namespaceName, metadataName);
```

为开放泛型定义保存 metadata 身份。该宏通常放在专用 definition 结构中；它本身不表示任何可直接使用的闭合类型。

```cpp
struct List_1_Definition
{
	GENERIC_CLASS_DEFINITION(
		"mscorlib", "System.Collections.Generic", "List`1");
};
```

### 3.2 `GENERIC_CLASS`

```cpp
GENERIC_CLASS(className, definition, managedArgumentTypes...);
```

在 C++ 类模板内声明闭合泛型引用类型。存储类型为当前闭合类的指针。

```cpp
template<typename T>
class List_1
{
public:
	GENERIC_CLASS(List_1, List_1_Definition, T);

	METHOD_INFO(0x06001234);
};
```

多参数泛型直接继续追加身份类型，例如 `GENERIC_CLASS(Dictionary_2, Definition, TKey, TValue)`。

### 3.3 `GENERIC_VALUE_CLASS`

```cpp
GENERIC_VALUE_CLASS(className, definition, managedArgumentTypes...);
```

声明闭合泛型值类型，存储类型为当前闭合类自身。典型用途是 `Nullable<T>` 或用户定义泛型结构体。

```cpp
struct Nullable_1_Definition
{
	GENERIC_CLASS_DEFINITION("mscorlib", "System", "Nullable`1");
};

template<typename T>
class Nullable_1
{
public:
	GENERIC_VALUE_CLASS(Nullable_1, Nullable_1_Definition, T);

	bool hasValue;
	Template::ManagedStorage<T> value;
};
```

`Template::ManagedStorage<T>` 会把身份类型转换为正确字段存储，例如引用类型变成指针、基础类型变成底层整数。

### 3.4 `CLOSED_GENERIC_CLASS_INFO`

```cpp
CLOSED_GENERIC_CLASS_INFO(definition, locator, managedArgumentTypes...);
```

为一个确实存在于目标 IL2CPP metadata 中的闭合泛型类注册定位器。宏应放在全局命名空间作用域，而不是类体或其他命名空间内。

```cpp
CLOSED_GENERIC_CLASS_INFO(
	List_1_Definition,
	VmGeneralType::RelativePointer<VmGeneralType::Class>::FromPointerRva(
		0x01234560U),
	UnityEngine::GameObject);

using GameObjectList = List_1<UnityEngine::GameObject>;
```

- `FromRva(rva)`：`GameAssembly + rva` 就是目标对象地址。
- `FromPointerRva(rva)`：`GameAssembly + rva` 是一个保存目标指针的槽位。

定位器可使用 `RelativePointer<VmGeneralType::Class>`；如果 UniDumper 提供的是 `Il2CppType*`，也可使用 `RelativePointer<VmGeneralType::Type>`，解析器会再转换为 class。

### 3.5 `CLOSED_GENERIC_METHOD_INFO`

```cpp
CLOSED_GENERIC_METHOD_INFO(
	declaring, token, locator, managedArgumentTypes...);
```

为某个泛型方法的真实闭合实例注册 `MethodInfo*` 定位器。宏应放在全局命名空间作用域。`declaring` 是声明方法的 SDK 类型，`token` 是泛型方法定义的 metadata token。

```cpp
namespace UnityEngine
{
	class Object
	{
	public:
		CLASS("UnityEngine.CoreModule", "UnityEngine", Object);

		template<typename T>
		static T* FindObjectOfType()
		{
			using MethodInfo = Template::ClosedGenericMethodInfo<
				Object, 0x06005678, T>;
			return Template::MethodInvoker<T*>(MethodInfo::GetMethod())();
		}
	};
}

CLOSED_GENERIC_METHOD_INFO(
	UnityEngine::Object,
	0x06005678,
	VmGeneralType::RelativePointer<VmGeneralType::Method>::FromPointerRva(
		0x02345670U),
	UnityEngine::GameObject);
```

定位器必须解析到闭合后的 `MethodInfo*`，不能只指向泛型方法定义或编译后函数地址。闭合泛型类上的非泛型方法仍使用 `METHOD_INFO`。

## 4. 嵌套类型宏

所有 `NESTED_*` 宏的 `declaring` 参数都是外层 SDK 类型，而不是字符串。

### 4.1 `NESTED_MANAGED_TYPE_NAMED`

```cpp
NESTED_MANAGED_TYPE_NAMED(
	declaring, className, metadataName, storageType);
```

嵌套类型的底层形式，显式指定外层类型、metadata 名和存储类型。

```cpp
class Player
{
public:
	CLASS("Assembly-CSharp", "Game", Player);

	class StateValue
	{
	public:
		NESTED_MANAGED_TYPE_NAMED(
			Player, StateValue, "State", std::int32_t);
	};
};
```

### 4.2 `NESTED_CLASS_NAMED`

```cpp
NESTED_CLASS_NAMED(declaring, className, metadataName);
```

声明名称被改写的嵌套引用类型，存储为 `className*`。

```cpp
class Player
{
public:
	CLASS("Assembly-CSharp", "Game", Player);

	class StateType
	{
	public:
		NESTED_CLASS_NAMED(Player, StateType, "State");
	};
};
```

### 4.3 `NESTED_CLASS`

```cpp
NESTED_CLASS(declaring, className);
```

声明 metadata 名与 C++ 名相同的嵌套引用类型。

```cpp
class Application
{
public:
	CLASS("UnityEngine.CoreModule", "UnityEngine", Application);

	class LogCallback
	{
	public:
		NESTED_CLASS(Application, LogCallback);
	};
};
```

### 4.4 `NESTED_VALUE_CLASS_NAMED`

```cpp
NESTED_VALUE_CLASS_NAMED(declaring, className, metadataName);
```

声明名称被改写的嵌套值类型，存储为 `className` 自身。

```cpp
class Player
{
public:
	CLASS("Assembly-CSharp", "Game", Player);

	class ModeValue
	{
	public:
		NESTED_VALUE_CLASS_NAMED(Player, ModeValue, "Mode");
		std::int32_t value;
	};
};
```

### 4.5 `NESTED_VALUE_CLASS`

```cpp
NESTED_VALUE_CLASS(declaring, className);
```

声明 metadata 名与 C++ 名相同的嵌套内联值类型。

```cpp
class Player
{
public:
	CLASS("Assembly-CSharp", "Game", Player);

	class State
	{
	public:
		NESTED_VALUE_CLASS(Player, State);
		std::int32_t id;
	};
};
```

### 4.6 `NESTED_VALUE_CLASS_STORAGE_NAMED`

```cpp
NESTED_VALUE_CLASS_STORAGE_NAMED(
	declaring, className, metadataName, storageType);
```

为名称被改写的嵌套基础类型或枚举包装指定底层存储。

```cpp
class Player
{
public:
	CLASS("Assembly-CSharp", "Game", Player);

	class StateValue
	{
	public:
		NESTED_VALUE_CLASS_STORAGE_NAMED(
			Player, StateValue, "State", std::int32_t);
	};
};
```

### 4.7 `NESTED_VALUE_CLASS_STORAGE`

```cpp
NESTED_VALUE_CLASS_STORAGE(declaring, className, storageType);
```

metadata 名自动取 `#className`，同时指定嵌套值类型的底层存储。

```cpp
class Player
{
public:
	CLASS("Assembly-CSharp", "Game", Player);

	class State
	{
	public:
		NESTED_VALUE_CLASS_STORAGE(Player, State, std::int32_t);
	};
};
```

## 5. 静态字段宏

当前版本只提供静态字段代理宏，没有 `FIELD` 或 `BACKING_FIELD` 实例字段宏。实例字段请使用本文第 7.3 节的底层接口。

### 5.1 `STATIC_FIELD_NAMED`

```cpp
STATIC_FIELD_NAMED(type, name, metadataName);
```

声明静态字段代理，并显式指定 metadata 字段名。适用于 C++ 关键字、非法标识符或改名后的字段。

```cpp
class Settings
{
public:
	CLASS("Assembly-CSharp", "Game", Settings);
	STATIC_FIELD_NAMED(std::int32_t, class_, "class");
};

std::int32_t value = Settings::class_;
Settings::class_ = 10;
```

### 5.2 `STATIC_FIELD`

```cpp
STATIC_FIELD(type, name);
```

声明 metadata 名与 C++ 成员名相同的静态字段代理。读取时自动转换到 `type`，赋值时写回 IL2CPP 静态字段。

```cpp
class GameSettings
{
public:
	CLASS("Assembly-CSharp", "Game", GameSettings);
	STATIC_FIELD(std::string, gameName);
	STATIC_FIELD(UnityEngine::GameObject*, currentObject);
};

std::string name = GameSettings::gameName;
GameSettings::gameName = "Sodium";
UnityEngine::GameObject* object = GameSettings::currentObject;
```

`std::string` 会自动与 IL2CPP `String` 转换。指针字段代理也支持 `GameSettings::currentObject->...`。

### 5.3 `STATIC_BACKING_FIELD_NAMED`

```cpp
STATIC_BACKING_FIELD_NAMED(type, name, metadataName);
```

声明自动属性的静态 backing field，并显式指定属性的 metadata 名。解析器会将 `metadataName` 转换为 `<metadataName>k__BackingField`，因此这里不要传完整 backing field 名。

```cpp
class GameSettings
{
public:
	CLASS("Assembly-CSharp", "Game", GameSettings);
	STATIC_BACKING_FIELD_NAMED(float, moveSpeed, "MoveSpeed");
};

float speed = GameSettings::moveSpeed; // 解析 <MoveSpeed>k__BackingField
```

### 5.4 `STATIC_BACKING_FIELD`

```cpp
STATIC_BACKING_FIELD(type, name);
```

属性 metadata 名自动取 `#name`，然后解析 `<name>k__BackingField`。

```cpp
class GameSettings
{
public:
	CLASS("Assembly-CSharp", "Game", GameSettings);
	STATIC_BACKING_FIELD(bool, IsDebug);
};

bool enabled = GameSettings::IsDebug;
GameSettings::IsDebug = true;
```

## 6. 方法宏

### 6.1 `METHOD_INFO`

```cpp
METHOD_INFO(token);
```

按 metadata token 声明一个静态方法信息缓存。生成的成员名是 `__<token>_Method_Info__`。例如 `METHOD_INFO(0x0600006D)` 生成 `__0x0600006D_Method_Info__`。

静态方法通过 `GetMethod()` 和 `il2cpp_runtime_invoke` 调用：

```cpp
class Application
{
public:
	CLASS("UnityEngine.CoreModule", "UnityEngine", Application);

	METHOD_INFO(0x0600006D);
	static std::string get_unityVersion()
	{
		return Template::MethodInvoker<std::string>(
			__0x0600006D_Method_Info__.GetMethod())();
	}
};
```

实例方法显式传入托管对象：

```cpp
METHOD_INFO(0x06001000);
void SetName(const std::string& name)
{
	Template::MethodInvoker<void, std::string> invoker(
		__0x06001000_Method_Info__.GetMethod());
	invoker.InvokeInstance(this, name);
}
```

虚方法使用 `InvokeVirtual(instance, ...)`。如需直接调用编译后函数，可把 `GetMethodAddress()` 传给 `MethodInvoker`，但此路径要求包装签名与目标 ABI 完全一致；普通 SDK 包装优先使用 `GetMethod()`。

## 7. 宏之外的常用接口

### 7.1 方法返回值、字符串与异常

`Template::MethodInvoker<R, Args...>` 支持：

- `operator()` / `Invoke()`：静态方法。
- `InvokeInstance(instance, ...)`：实例方法。
- `InvokeVirtual(instance, ...)`：按对象运行时类型分派虚方法。
- `std::string` 参数与返回值：自动和 IL2CPP `String` 转换。

托管方法抛出异常时会转为 `Template::ManagedInvocationException`；`Exception()` 可取得原始托管异常对象。绑定缺失、线程附加失败或无效 `MethodInfo` 会抛出 `std::runtime_error`。

### 7.2 创建对象

```cpp
UnityEngine::GameObject* object =
	Template::NewObject<UnityEngine::GameObject>();
```

引用类型返回 `T*`。值类型返回 `Template::BoxedValue<T>*`，使用 `Value()` 访问装箱后的值区域。分配只创建对象，构造函数仍需通过对应 SDK 方法包装调用。

### 7.3 访问实例字段

当前没有实例字段宏，使用 `UniResolver.GetField` 和 `VmGeneralType::api`：

```cpp
auto playerClass = Player::ThisClassInfo.Instance();
auto healthField = UniResolver.GetField(playerClass, "health");

std::int32_t health = 0;
VmGeneralType::api.GetFieldValue(
	reinterpret_cast<VmGeneralType::Object>(player),
	healthField,
	&health);

health = 100;
VmGeneralType::api.SetFieldValue(
	reinterpret_cast<VmGeneralType::Object>(player),
	healthField,
	&health);
```

### 7.4 名称映射

名称映射方向始终是“SDK 中的逻辑原名 → 运行时 metadata 实际名”。必须在任何宏首次解析类、字段或方法之前安装：

```cpp
UniResolverContext::NameMap nameMap;
nameMap.MapAssembly("Assembly-CSharp", "a");
nameMap.MapNamespace("Assembly-CSharp", "Game", "b");
nameMap.MapClass("Assembly-CSharp", "Game", "Player", "c");
nameMap.MapNestedClass("Assembly-CSharp", "Game", "Player", "State", "d");
nameMap.MapField("Assembly-CSharp", "Game", "Player", "health", "e");
nameMap.MapMethod("Assembly-CSharp", "Game", "Player", "Move", "f");
nameMap.MapMethod(
	"Assembly-CSharp",
	"Game",
	"Player",
	"System.Void",
	"Move",
	{ "System.Single" },
	"g");
nameMap.MapType("Game.Player", "b.c");

UniResolver.SetNameMap(std::move(nameMap));
UniResolver.Setup();
```

完整签名的方法映射优先于仅按方法名的映射。未映射的名称原样回退。`SetNameMap` 和 `ClearNameMap` 会清空上下文缓存，但类型、字段和方法宏还持有各自的静态解析缓存，因此不要在这些宏已完成首次解析后更换映射。

### 7.5 生命周期与线程

- `UniResolver.Setup()` 绑定 `GameAssembly.dll` API、取得 domain，并在需要时附加当前线程。
- 解析、字段访问、字符串转换、对象分配和方法调用会确保当前调用线程已附加。
- `UniResolver.Destroy()` 清空上下文缓存，并且只分离由 `Setup()` 自己附加的线程。
- DLL 应在 `DllMain` 创建的工作线程中执行初始化和 SDK 调用，不要直接在 loader lock 下做解析或托管调用。

可运行的最小 DLL 接入方式见 [example/main.cpp](example/main.cpp)，对应的 UniDumper SDK 片段见 [example/UniDumperSdk.h](example/UniDumperSdk.h)。
