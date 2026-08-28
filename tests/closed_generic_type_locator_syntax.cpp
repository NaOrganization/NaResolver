#include "../UniResolver.h"

namespace LocatorSyntaxTest
{
class Definition
{
public:
	CLASS("Assembly-CSharp", "LocatorSyntaxTest", Definition);
};

class Argument
{
public:
	CLASS("Assembly-CSharp", "LocatorSyntaxTest", Argument);
};
}

CLOSED_GENERIC_CLASS_INFO(
	LocatorSyntaxTest::Definition,
	VmGeneralType::RelativePointer<VmGeneralType::Type>::FromRva(0x1234),
	LocatorSyntaxTest::Argument);

void ResolveClosedGenericTypeLocatorSyntax()
{
	static_cast<void>(Template::ClosedGenericClassInfo<
		LocatorSyntaxTest::Definition,
		LocatorSyntaxTest::Argument>::Instance());
}
