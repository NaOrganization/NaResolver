#include <Windows.h>

#include <array>
#include <atomic>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../KrOr.h"

#undef GetClassName
#define private public
#include "../VmApi.h"
#include "../UniResolver.h"
#undef private

struct RuntimeIdentity
{
	CLASS("TestAssembly", "Test", RuntimeIdentity);
	STATIC_FIELD(std::int32_t, Counter);
	METHOD_INFO(0x06000001);
};

namespace
{
struct ReferenceIdentity
{
	using ManagedStorageType = ReferenceIdentity*;
};

struct ValueIdentity
{
	using ManagedStorageType = ValueIdentity;
	std::int32_t value = 0;
};

static_assert(std::same_as<Template::NewObjectResult<ReferenceIdentity>, ReferenceIdentity*>);
static_assert(std::same_as<Template::NewObjectResult<ValueIdentity>,
	Template::BoxedValue<ValueIdentity>*>);
static_assert(offsetof(Template::BoxedValue<ValueIdentity>, value) >= sizeof(void*) * 2U);

VmGeneralType::Method gExpectedMethod = nullptr;
VmGeneralType::Method gVirtualMethod = nullptr;
VmGeneralType::Object gExpectedInstance = nullptr;
VmGeneralType::Object gManagedException = nullptr;
VmGeneralType::Object gBoxedValue = nullptr;
VmGeneralType::String gManagedString = nullptr;
std::int32_t gReturnValue = 73;
std::u16string gReturnString = u"resolved";
std::string gReceivedString;
std::int32_t gReceivedValue = 0;
std::int32_t* gReceivedByReference = nullptr;
bool gReturnManagedString = false;
thread_local bool gThreadAttached = false;
std::atomic_int gAttachCount = 0;
std::atomic_int gDetachCount = 0;
std::atomic_bool gAllowAttach = true;
VmGeneralType::Object gGcTarget = nullptr;
std::int32_t gStaticValue = 0;

VmGeneralType::Domain GetDomain()
{
	return reinterpret_cast<VmGeneralType::Domain>(0x7070);
}

VmGeneralType::Thread GetCurrentThread()
{
	return gThreadAttached ? reinterpret_cast<VmGeneralType::Thread>(0x8080) : nullptr;
}

VmGeneralType::Thread AttachThread(VmGeneralType::Domain domain)
{
	assert(domain != nullptr);
	if (!gAllowAttach.load()) return nullptr;
	gThreadAttached = true;
	++gAttachCount;
	return reinterpret_cast<VmGeneralType::Thread>(0x8080);
}

VmGeneralType::Assembly OpenAssembly(VmGeneralType::Domain, const char*)
{
	assert(gThreadAttached);
	return reinterpret_cast<VmGeneralType::Assembly>(0x9090);
}

VmGeneralType::Image GetAssemblyImage(VmGeneralType::Assembly)
{
	assert(gThreadAttached);
	return reinterpret_cast<VmGeneralType::Image>(0xA0A0);
}

VmGeneralType::Class FindClass(VmGeneralType::Image, const char*, const char*)
{
	assert(gThreadAttached);
	return reinterpret_cast<VmGeneralType::Class>(0xB0B0);
}

VmGeneralType::Class FindMissingClass(VmGeneralType::Image, const char*, const char*)
{
	assert(gThreadAttached);
	return nullptr;
}

VmGeneralType::Type GetClassType(VmGeneralType::Class)
{
	assert(gThreadAttached);
	return reinterpret_cast<VmGeneralType::Type>(0xC0C0);
}

VmGeneralType::Method GetNextMethod(VmGeneralType::Class, void** iterator)
{
	assert(gThreadAttached);
	if (*iterator != nullptr) return nullptr;
	*iterator = reinterpret_cast<void*>(1);
	return gExpectedMethod;
}

std::uint32_t GetMethodToken(VmGeneralType::Method)
{
	assert(gThreadAttached);
	return 0x06000001U;
}

const char* GetMethodName(VmGeneralType::Method)
{
	assert(gThreadAttached);
	return "Resolve";
}

VmGeneralType::Type GetMethodReturnType(VmGeneralType::Method)
{
	assert(gThreadAttached);
	return reinterpret_cast<VmGeneralType::Type>(0xD0D0);
}

std::uint32_t GetMethodParameterCount(VmGeneralType::Method)
{
	assert(gThreadAttached);
	return 0U;
}

VmGeneralType::Type GetMethodParameter(VmGeneralType::Method, std::uint32_t)
{
	assert(false);
	return nullptr;
}

char* GetTypeName(VmGeneralType::Type)
{
	assert(gThreadAttached);
	return const_cast<char*>("System.Void");
}

VmGeneralType::Field FindField(VmGeneralType::Class, const char*)
{
	assert(gThreadAttached);
	return reinterpret_cast<VmGeneralType::Field>(0xE0E0);
}

void GetStaticFieldValue(VmGeneralType::Field, void* value)
{
	assert(gThreadAttached);
	*static_cast<std::int32_t*>(value) = gStaticValue;
}

void SetStaticFieldValue(VmGeneralType::Field, void* value)
{
	assert(gThreadAttached);
	gStaticValue = *static_cast<std::int32_t*>(value);
}

VmGeneralType::Object ObjectNew(VmGeneralType::Class)
{
	assert(gThreadAttached);
	return nullptr;
}

void DetachThread(VmGeneralType::Thread)
{
	assert(gThreadAttached);
	gThreadAttached = false;
	++gDetachCount;
}

std::uint32_t NewGcHandle(VmGeneralType::Object object, bool)
{
	gGcTarget = object;
	return 1U;
}

VmGeneralType::Object GetGcHandleTarget(std::uint32_t handle)
{
	return handle == 1U ? gGcTarget : nullptr;
}

void FreeGcHandle(std::uint32_t handle)
{
	if (handle == 1U) gGcTarget = nullptr;
}

VmGeneralType::String NewString(const char* value, const std::uint32_t length)
{
	assert(gThreadAttached);
	gReceivedString.assign(value, length);
	return gManagedString;
}

const char16_t* StringChars(VmGeneralType::String)
{
	assert(gThreadAttached);
	return gReturnString.data();
}

std::int32_t StringLength(VmGeneralType::String)
{
	assert(gThreadAttached);
	return static_cast<std::int32_t>(gReturnString.size());
}

void* Unbox(VmGeneralType::Object object)
{
	return object == gBoxedValue ? &gReturnValue : nullptr;
}

VmGeneralType::Method GetVirtualMethod(
	VmGeneralType::Object instance, VmGeneralType::Method method)
{
	assert(gThreadAttached);
	assert(instance == gExpectedInstance);
	assert(method == gExpectedMethod);
	return gVirtualMethod;
}

VmGeneralType::Object RuntimeInvoke(VmGeneralType::Method method,
	VmGeneralType::Object instance, void** parameters,
	VmGeneralType::Object* exception)
{
	assert(gThreadAttached);
	assert(method == gExpectedMethod || method == gVirtualMethod);
	assert(instance == gExpectedInstance || instance == nullptr);
	if (gReturnManagedString)
	{
		assert(parameters != nullptr);
		assert(parameters[0] == gManagedString);
	}
	else if (parameters != nullptr)
	{
		gReceivedValue = *static_cast<std::int32_t*>(parameters[0]);
		gReceivedByReference = static_cast<std::int32_t*>(parameters[1]);
	}
	if (exception != nullptr) *exception = gManagedException;
	return gReturnManagedString ? reinterpret_cast<VmGeneralType::Object>(gManagedString) :
		gBoxedValue;
}

std::int32_t RawAddressCall(const std::int32_t value)
{
	return value + 9;
}

VmGeneralType::String RawStringCall(VmGeneralType::String value)
{
	assert(gThreadAttached);
	assert(value == gManagedString);
	return value;
}

void ResetApi()
{
	VmGeneralType::api = VmGeneralType::Api{};
	gExpectedMethod = reinterpret_cast<VmGeneralType::Method>(0x1010);
	gVirtualMethod = reinterpret_cast<VmGeneralType::Method>(0x2020);
	gExpectedInstance = reinterpret_cast<VmGeneralType::Object>(0x3030);
	gManagedException = nullptr;
	gBoxedValue = reinterpret_cast<VmGeneralType::Object>(0x4040);
	gManagedString = reinterpret_cast<VmGeneralType::String>(0x5050);
	gReceivedString.clear();
	gReceivedValue = 0;
	gReceivedByReference = nullptr;
	gReturnManagedString = false;
	gStaticValue = 0;
	gThreadAttached = false;
	gAllowAttach.store(true);
	VmGeneralType::api.domainGet = &GetDomain;
	VmGeneralType::api.threadCurrent = &GetCurrentThread;
	VmGeneralType::api.threadAttach = &AttachThread;
	VmGeneralType::api.threadDetach = &DetachThread;
	VmGeneralType::api.gcHandleNew = &NewGcHandle;
	VmGeneralType::api.gcHandleFree = &FreeGcHandle;
	VmGeneralType::api.gcHandleGetTarget = &GetGcHandleTarget;
}

void SetRequiredBindings(VmGeneralType::Api& api)
{
#define SET_REQUIRED(name) api.name = reinterpret_cast<decltype(api.name)>(&GetDomain)
	SET_REQUIRED(domainGet);
	SET_REQUIRED(assemblyOpen);
	SET_REQUIRED(assemblyGetImage);
	SET_REQUIRED(classFromName);
	SET_REQUIRED(classGetMethods);
	SET_REQUIRED(classGetField);
	SET_REQUIRED(classGetType);
	SET_REQUIRED(classFromType);
	SET_REQUIRED(fieldGetValue);
	SET_REQUIRED(fieldSetValue);
	SET_REQUIRED(staticFieldGetValue);
	SET_REQUIRED(staticFieldSetValue);
	SET_REQUIRED(methodGetName);
	SET_REQUIRED(methodGetToken);
	SET_REQUIRED(methodGetReturnType);
	SET_REQUIRED(methodGetParamCount);
	SET_REQUIRED(methodGetParam);
	SET_REQUIRED(typeGetName);
	SET_REQUIRED(stringNewLen);
	SET_REQUIRED(stringChars);
	SET_REQUIRED(stringLength);
	SET_REQUIRED(threadAttach);
	SET_REQUIRED(threadCurrent);
	SET_REQUIRED(threadDetach);
	SET_REQUIRED(runtimeInvoke);
	SET_REQUIRED(objectUnbox);
	SET_REQUIRED(gcHandleNew);
	SET_REQUIRED(gcHandleFree);
	SET_REQUIRED(gcHandleGetTarget);
	SET_REQUIRED(objectNew);
	SET_REQUIRED(objectGetVirtualMethod);
#undef SET_REQUIRED
}
}

int main()
{
	VmGeneralType::Api bindingProbe = {};
	assert(!bindingProbe.BindIl2Cpp(GetModuleHandleW(nullptr)));
	SetRequiredBindings(bindingProbe);
	assert(bindingProbe.HasRequiredBindings());
	bindingProbe.threadCurrent = nullptr;
	assert(!bindingProbe.HasRequiredBindings());
	SetRequiredBindings(bindingProbe);
	bindingProbe.staticFieldSetValue = nullptr;
	assert(!bindingProbe.HasRequiredBindings());

	ResetApi();
	VmGeneralType::api.runtimeInvoke = &RuntimeInvoke;
	VmGeneralType::api.objectUnbox = &Unbox;
	VmGeneralType::api.stringNewLen = &NewString;
	VmGeneralType::api.stringChars = &StringChars;
	VmGeneralType::api.stringLength = &StringLength;
	VmGeneralType::api.objectGetVirtualMethod = &GetVirtualMethod;

	std::int32_t byReference = 41;
	Template::MethodInvoker<std::int32_t, std::int32_t, std::int32_t*>
		instanceInvoker(gExpectedMethod);
	assert(instanceInvoker.InvokeInstance(
		gExpectedInstance, 17, &byReference) == gReturnValue);
	assert(gReceivedValue == 17);
	assert(gReceivedByReference == &byReference);

	assert(instanceInvoker.InvokeVirtual(
		gExpectedInstance, 19, &byReference) == gReturnValue);
	assert(gReceivedValue == 19);

	gReturnManagedString = true;
	Template::MethodInvoker<std::string, const std::string&>
		stringInvoker(gExpectedMethod);
	assert(stringInvoker(std::string("input")) == "resolved");
	assert(gReceivedString == "input");

	gReturnManagedString = false;
	gManagedException = reinterpret_cast<VmGeneralType::Object>(0x6060);
	try
	{
		(void)instanceInvoker.InvokeInstance(
			gExpectedInstance, 23, &byReference);
		assert(false);
	}
	catch (const Template::ManagedInvocationException& exception)
	{
		assert(exception.Exception() == gManagedException);
		assert(instanceInvoker.GetLastException() == gManagedException);
	}
	assert(gGcTarget == nullptr);
	assert(gAttachCount.load() == gDetachCount.load());

	ResetApi();
	VmGeneralType::api.assemblyOpen = &OpenAssembly;
	VmGeneralType::api.assemblyGetImage = &GetAssemblyImage;
	VmGeneralType::api.classFromName = &FindClass;
	VmGeneralType::api.classGetType = &GetClassType;
	VmGeneralType::api.classGetMethods = &GetNextMethod;
	VmGeneralType::api.methodGetToken = &GetMethodToken;
	VmGeneralType::api.methodGetName = &GetMethodName;
	VmGeneralType::api.methodGetReturnType = &GetMethodReturnType;
	VmGeneralType::api.methodGetParamCount = &GetMethodParameterCount;
	VmGeneralType::api.methodGetParam = &GetMethodParameter;
	VmGeneralType::api.typeGetName = &GetTypeName;
	VmGeneralType::api.classGetField = &FindField;
	VmGeneralType::api.staticFieldGetValue = &GetStaticFieldValue;
	VmGeneralType::api.staticFieldSetValue = &SetStaticFieldValue;
	VmGeneralType::api.objectGetVirtualMethod = &GetVirtualMethod;
	VmGeneralType::api.objectNew = &ObjectNew;
	naResolverInstance.domain = GetDomain();
	naResolverInstance.cache.Clear();
	std::thread firstManagedUse([]
	{
		const UniResolver::Class klass = RuntimeIdentity::ThisClassInfo.Instance();
		assert(klass.klass != nullptr);
		assert(RuntimeIdentity::__0x06000001_Method_Info__.GetMethod() == gExpectedMethod);
		assert(RuntimeIdentity::__0x06000001_Method_Info__.GetVirtualMethod(
			gExpectedInstance) == gVirtualMethod);
		RuntimeIdentity::Counter = 91;
		assert(static_cast<std::int32_t>(RuntimeIdentity::Counter) == 91);
		try
		{
			(void)Template::NewObject<RuntimeIdentity>();
			assert(false);
		}
		catch (const std::runtime_error& exception)
		{
			assert(std::string(exception.what()).find("allocate") != std::string::npos);
		}
	});
	firstManagedUse.join();
	assert(gAttachCount.load() == gDetachCount.load());

	ResetApi();
	gAllowAttach.store(false);
	try
	{
		(void)Template::NewObject<RuntimeIdentity>();
		assert(false);
	}
	catch (const std::runtime_error& exception)
	{
		assert(std::string(exception.what()).find("attach") != std::string::npos);
	}

	ResetApi();
	VmGeneralType::api.assemblyOpen = &OpenAssembly;
	VmGeneralType::api.assemblyGetImage = &GetAssemblyImage;
	VmGeneralType::api.classFromName = &FindMissingClass;
	naResolverInstance.domain = GetDomain();
	naResolverInstance.cache.Clear();
	decltype(RuntimeIdentity::ThisClassInfo)::ClassInfoCache = {};
	try
	{
		(void)Template::NewObject<RuntimeIdentity>();
		assert(false);
	}
	catch (const std::runtime_error& exception)
	{
		assert(std::string(exception.what()).find("resolve") != std::string::npos);
	}
	assert(gAttachCount.load() == gDetachCount.load());

	ResetApi();
	try
	{
		(void)Template::MethodInvoker<void>(gExpectedMethod)();
		assert(false);
	}
	catch (const std::runtime_error& exception)
	{
		assert(std::string(exception.what()).find("il2cpp_runtime_invoke") !=
			std::string::npos);
	}

	Template::MethodInvoker<std::int32_t, std::int32_t> directInvoker(
		reinterpret_cast<void*>(&RawAddressCall));
	assert(directInvoker(4) == 13);

	ResetApi();
	VmGeneralType::api.stringNewLen = &NewString;
	VmGeneralType::api.stringChars = &StringChars;
	VmGeneralType::api.stringLength = &StringLength;
	Template::MethodInvoker<std::string, const std::string&> directStringInvoker(
		reinterpret_cast<void*>(&RawStringCall));
	assert(directStringInvoker("raw") == "resolved");
	assert(gReceivedString == "raw");
	assert(gAttachCount.load() == gDetachCount.load());
}
