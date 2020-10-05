#include <catch2/catch.hpp>
#include "result.hpp"


using namespace Wrench;


struct TScopeGuard
{
	TScopeGuard(const std::function<void()>& action) : mAction(action) {}
	~TScopeGuard()
	{
		mAction();
	}

	std::function<void()> mAction;
};


TEST_CASE("Result<T,E> Tests")
{
	SECTION("TestConstructor_CreateValueObjects_CorrectlyInitializesThem")
	{
		REQUIRE(Result<int, int>(TOkValue<int>(42)).IsOk()); // built-in type

		Result<float, int> r = TOkValue<float>(0.42f); // this case is also possible
		REQUIRE((r.IsOk() && (fabs(r.Get() - 0.42f) < 1e-3f)));

		// \todo some POD type

		bool hasObjectBeenDestroyed = false;

		REQUIRE(Result<TScopeGuard, int>(TOkValue<TScopeGuard>(TScopeGuard([&hasObjectBeenDestroyed] { hasObjectBeenDestroyed = true; }))).IsOk()); // non POD type with destructor should be called
		REQUIRE(hasObjectBeenDestroyed);
	}

	SECTION("TestConstructor_CreateInvalidObjects_CorrectlyInitializesThem")
	{
		REQUIRE(!Result<int, int>(TErrValue<int>(42)).IsOk());
	}

}