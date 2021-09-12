#include <catch2/catch.hpp>
#define DEFER_IMPLEMENTATION
#include "deferOperation.hpp"


using namespace Wrench;


TEST_CASE("Test defer operation")
{
	SECTION("TestDeferBehaviour_CreateSimpleDefer_ExecutesLogicWhenGoesOutOfScope")
	{
		bool deferExecuted = false;

		{
			defer([&deferExecuted] { deferExecuted = true; });
		}

		REQUIRE(deferExecuted);
	}

	SECTION("TestDeferBehaviour_CreateFewDeferOperations_CreateNoCollisionsExecutedInSequentialOrder")
	{
		defer([] {}); defer([] {}); 
		defer([] {});
	}
}