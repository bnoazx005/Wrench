#include <catch2/catch.hpp>
#define RAND_UTILS_IMPLEMENTATION
#include "randomUtils.hpp"


using namespace Wrench;


TEST_CASE("randomUtils Tests")
{
	SECTION("TestGet_PassIntegerRange_ReturnsRandomValueThatLiesBetweenTheBorders")
	{
		constexpr int left = -100;
		constexpr int right = 100;

		DefaultRandom rand(0x42);
		
		for (size_t i = 0; i < 1000; i++)
		{
			const int randValue = rand.Get(left, right);
			REQUIRE((randValue >= left && randValue <= right));
		}
	}

	SECTION("TestGet_PassRealRange_ReturnsRandomValueThatLiesBetweenTheBorders")
	{
		constexpr float left = -100;
		constexpr float right = 100;

		DefaultRandom rand(0x42);

		for (size_t i = 0; i < 1000; i++)
		{
			const float randValue = rand.Get(left, right);
			REQUIRE((randValue >= left && randValue <= right));
		}
	}
}