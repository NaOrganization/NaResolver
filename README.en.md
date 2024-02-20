#  NaResolver V2

"Lightweight Unity Game plugin development framework，Give you a excellent experience."

------

### Introduction

**NaResolver** is a lightweight development framework for plugin development.

### TODO

- [ ] Simplify Method Invoker related code

### The Pitch

- Lightweight

- Setup easily
- Excellent compatibility
- Support Mono and Il2Cpp **(New)**

### Usage

####  How to setup？

```cpp
#define NA_RESOLVER_STRING_XOR _xor_ // if you need use string xor
#include <NaResolver/NaResolver.h>

using namespace NaOrganization::MidTerm;

Simplify xxx related code

...main()
{
	if (UnityResolver.Setup())
        printf("Setup successfully!");
}
```

#### How to get class, method？

```cpp
// class
NaResolver::Class* klass = UnityResolver.GetClass("assembly", "namespace", "className");

// method
UnityResolver.GetMethod(klass, "returnType", "methodName", {"parameterType1", "parameterType2"});
```

#### How to use them

```cpp
// class
printf("(NaResolver->GetClass) Class(%p): %p\n", gameObject.type, gameObject.klass);

// method
printf("(NaResolver->GetMethod) Method(%p): %p\n", get_transform.method.GetInvokeAddress(), get_transform.method);
void* __this = nullptr; // fake value for test
void* result = NaApiInvoker<void*, void*>(get_transform.method.GetInvokeAddress()).Invoke(__this);
```

## License

**NaResolver** is licensed under the GPL3.0 License, see [LICENSE.txt](LICENSE.txt) for more information.
