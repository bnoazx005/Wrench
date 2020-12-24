#include <catch2/catch.hpp>
#include "variant.hpp"


using namespace Wrench;


TEST_CASE("Variant Tests")
{
	SECTION("TestTMaxSize_PassTypes_ReturnsSizeOfLongestOne")
	{
		REQUIRE(TMaxSize<int, char>::mValue == sizeof(int));
	}

	SECTION("TestFindType_PassTypes_ReturnIndexOfType")
	{
		REQUIRE(TFindType<int, float, std::string, char, int>::mValue == 3); 
		REQUIRE(TFindType<int, float, std::string, char>::mValue == 3); // 3 means there is no given type
	}

	SECTION("TestVariant_CreateSomeObjects_CorrectlyInitializesInternalState")
	{
		Variant<int, float, std::string> t;
		t = 5.0f;

		REQUIRE(t.Is<float>());
		REQUIRE((!t.Is<char>() && !t.Is<int>()));

		t = std::string("tttttt");

		REQUIRE(t.Is<std::string>());
	}

	SECTION("TestAs_CreateStringVariantAndTryToGetItsValueWithAs_ReturnsStringBack")
	{
		const std::string expectedStr("Test");

		Variant<std::string> v = MakeVariant<std::string, std::string>(expectedStr);

		REQUIRE((v.Is<std::string>() && v.As<std::string>() == expectedStr));
	}
}