#include <catch2/catch.hpp>
#define STR_UTILS_IMPLEMENTATION
#include "stringUtils.hpp"


using namespace Wrench;


TEST_CASE("stringUtils Tests")
{
	SECTION("TestRemoveExtraWhitespaces_PassEmptyString_ReturnsEmptyString")
	{
		REQUIRE(StringUtils::RemoveExtraWhitespaces(StringUtils::GetEmptyStr()) == StringUtils::GetEmptyStr());
	}

	SECTION("TestRemoveExtraWhitespaces_PassStringWithRedundantSpaces_ReturnsStringWhereAllWordsDelimitedWithSingleSpace")
	{
		auto testCases =
		{
			std::tuple<std::string, std::string> { "Hello     ,   World!", "Hello , World!" }, // expected, actual
			std::tuple<std::string, std::string> { "Test", "Test" },
			std::tuple<std::string, std::string> { "This is a test", "This is a test" },
		};

		for (auto&& currTestCase : testCases)
		{
			REQUIRE(StringUtils::RemoveExtraWhitespaces(std::get<0>(currTestCase)) == std::get<1>(currTestCase));
		}
	}

	SECTION("TestRemoveAllWhitespaces_PassEmptyString_ReturnsEmptyString")
	{
		REQUIRE(StringUtils::RemoveAllWhitespaces(StringUtils::GetEmptyStr()) == StringUtils::GetEmptyStr());
	}

	SECTION("TestRemoveAllWhitespaces_PassStringWithSpaces_ReturnsStringWithRemovedOnes")
	{
		auto testCases =
		{
			std::tuple<std::string, std::string> { "Hello     ,   World!", "Hello,World!" }, // expected, actual
			std::tuple<std::string, std::string> { "Test", "Test" },
			std::tuple<std::string, std::string> { "This is a test", "Thisisatest" },
		};

		for (auto&& currTestCase : testCases)
		{
			REQUIRE(StringUtils::RemoveAllWhitespaces(std::get<0>(currTestCase)) == std::get<1>(currTestCase));
		}
	}

	SECTION("TestGetEmptyStr_ReturnsEmptyString")
	{
		REQUIRE(StringUtils::GetEmptyStr() == std::string{});
	}

	SECTION("TestStartsWith_PassNonEmptyString_ReturnsTrueIfThatContainsSubstr")
	{
		auto testCases =
		{
			std::tuple<std::string, std::string, bool> { "This is a test", "This", true }, // expected, actual
			std::tuple<std::string, std::string, bool> { "   ", " ", true },
			std::tuple<std::string, std::string, bool> { "", "", true },
			std::tuple<std::string, std::string, bool> { "Test", " ", false },
			std::tuple<std::string, std::string, bool> { "Test", "Testtt", false },
		};

		for (auto&& currTestCase : testCases)
		{
			REQUIRE(StringUtils::StartsWith(std::get<0>(currTestCase), std::get<1>(currTestCase)) == std::get<bool>(currTestCase));
		}
	}

	SECTION("TestReplaceAll_PassStringWithoutReplacingSubstrs_ReturnsOriginalString")
	{
		REQUIRE("test" == StringUtils::ReplaceAll("test", "1", "2"));
	}

	SECTION("TestReplaceAll_PassSubstringThatPartiallyMatches_ReturnsOriginalString")
	{
		REQUIRE("/" == StringUtils::ReplaceAll("/", "//", "."));
	}

	SECTION("TestEndsWith_PassEmptyString_ReturnsFalse")
	{
		REQUIRE(!StringUtils::EndsWith(StringUtils::GetEmptyStr(), "test"));
	}

	SECTION("TestEndsWith_PassStringWithSuffix_ReturnsTrue")
	{
		REQUIRE(StringUtils::EndsWith("test123", "123"));
	}
}