#include "../UniResolver.h"

#include <cassert>

struct StaticFieldTarget
{
	int value = 0;
};

class StaticFieldOwner
{
public:
	CLASS("Assembly-CSharp", "Tests", StaticFieldOwner);
	STATIC_FIELD(StaticFieldTarget*, target);
};

static_assert(std::is_same_v<decltype(StaticFieldOwner::target.operator->()), StaticFieldTarget*>);
static_assert(std::is_same_v<decltype(StaticFieldOwner::target->value), int>);

int main()
{
	UniResolverContext::NameMap map;
	assert(map.Empty());
	assert(map.RelocateClass("Assembly-CSharp", "Game", "Player") == "Player");

	map.MapAssembly("Assembly-CSharp", "a");
	map.MapNamespace("Assembly-CSharp", "Game", "b");
	map.MapClass("Assembly-CSharp", "Game", "Player", "c");
	map.MapNestedClass("Assembly-CSharp", "Game", "Player", "State", "d");
	map.MapField("Assembly-CSharp", "Game", "Player", "health", "e");
	map.MapType("Game.Player", "b.c");
	map.MapMethod("Assembly-CSharp", "Game", "Player", "Move", "f");
	map.MapMethod(
		"Assembly-CSharp",
		"Game",
		"Player",
		"System.Void",
		"Move",
		{ "System.Single" },
		"g");

	assert(!map.Empty());
	assert(map.RelocateAssembly("Assembly-CSharp") == "a");
	assert(map.RelocateNamespace("Assembly-CSharp", "Game") == "b");
	assert(map.RelocateClass("Assembly-CSharp", "Game", "Player") == "c");
	assert(map.RelocateNestedClass("Assembly-CSharp", "Game", "Player", "State") == "d");
	assert(map.RelocateField("Assembly-CSharp", "Game", "Player", "health") == "e");
	assert(map.RelocateType("Game.Player") == "b.c");
	assert(map.RelocateMethod(
		"Assembly-CSharp",
		"Game",
		"Player",
		"System.Void",
		"Move",
		{ "System.Single" }) == "g");
	assert(map.RelocateMethod(
		"Assembly-CSharp",
		"Game",
		"Player",
		"System.Void",
		"Move",
		{ "System.Int32" }) == "f");

	UniResolverContext resolver;
	resolver.SetNameMap(map);
	assert(resolver.GetNameMap().RelocateClass("Assembly-CSharp", "Game", "Player") == "c");
	resolver.ClearNameMap();
	assert(resolver.GetNameMap().Empty());
}
