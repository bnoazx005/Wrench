#include <catch2/catch.hpp>
#include "variant.hpp"


using namespace Wrench;


TEST_CASE("Variant Tests")
{
	SECTION("TestTMaxSize_PassTypes_ReturnsSizeOfLongestOne")
	{
		REQUIRE(GetMaxSize<int, char>() == sizeof(int));
		REQUIRE(GetMaxSize<int>() == sizeof(int));
	}

	SECTION("TestFindType_PassTypes_ReturnIndexOfType")
	{
		REQUIRE(GetIndexOfType<int, float, std::string, char, int>() == 3);
		REQUIRE(GetIndexOfType<int, float, std::string, char>() == (std::numeric_limits<size_t>::max)()); // 3 means there is no given type
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

	SECTION("TestVariant_PassNonPODType_CorrectlyDestructsIt")
	{
		struct TScopeGuard
		{
			TScopeGuard(const std::function<void()>& action) : mAction(action) {}
			~TScopeGuard()
			{
				mAction();
			}

			std::function<void()> mAction;
		};

		bool hasDestroyed = false;

		{
			Variant<TScopeGuard> v = MakeVariant<TScopeGuard, TScopeGuard>(TScopeGuard([&hasDestroyed] { hasDestroyed = true; }));
		}

		REQUIRE(hasDestroyed);
	}
}