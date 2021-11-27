#include <catch2/catch.hpp>
#define DELEGATE_IMPLEMENTATION
#include "delegate.hpp"


using namespace Wrench;


bool HasTestFloatFunctionBeenExecuted = false;


void TestFloatFunction(float value)
{
	HasTestFloatFunctionBeenExecuted = true;
}


TEST_CASE("Test Delegate")
{
	HasTestFloatFunctionBeenExecuted = false;
	
	SECTION("TestSubscribe_PassSimpleFunction_CorrectlyCreatesDelegate")
	{
		Delegate<float> testDelegate;
		testDelegate.Subscribe(TestFloatFunction);

		testDelegate.Notify(0.0f);

		REQUIRE(HasTestFloatFunctionBeenExecuted);
	}

	SECTION("TestSubscribe_PassLambdaFunction_CorrectlyCreatesDelegate")
	{
		bool hasLambdaBeenExecuted = false;

		Delegate<float> testDelegate;
		testDelegate.Subscribe([&](float) { hasLambdaBeenExecuted = true; });

		testDelegate.Notify(0.0f);

		REQUIRE(hasLambdaBeenExecuted);
	}

	SECTION("TestSubscribe_PassStructMember_CorrectlyCreatesDelegate")
	{
		bool hasLambdaBeenExecuted = false;

		struct Foo
		{
			Foo(bool& flag) { mpFlag = &flag; }
			void Bar(float) { *mpFlag = true; }

			bool* mpFlag = nullptr;
		};

		Delegate<float> testDelegate;
		
		Foo testFooObject(hasLambdaBeenExecuted);
		
		testDelegate.Subscribe(std::bind(&Foo::Bar, &testFooObject, std::placeholders::_1));

		testDelegate.Notify(0.0f);

		REQUIRE(hasLambdaBeenExecuted);
	}

	/// \note This is constraint caused by std::function which has no well defined equality operator
	SECTION("TestSubscribe_PassSameLambdaFunctionTwoTimes_ReturnsDifferentHandles")
	{
		bool hasLambdaBeenExecuted = false;

		Delegate<float> testDelegate;

		auto function = [&](float) { hasLambdaBeenExecuted = true; };

		auto firstHandle  = testDelegate.Subscribe(function);
		auto secondHandle = testDelegate.Subscribe(function);

		REQUIRE(firstHandle != secondHandle);
	}

	SECTION("TestUnsubscribe_PassLambdaFunctionAndUnsubscribeIt_CorrectlyDoesThat")
	{
		bool hasLambdaBeenExecuted = false;

		Delegate<float> testDelegate;

		auto function = [&](float) { hasLambdaBeenExecuted = true; };

		auto firstHandle = testDelegate.Subscribe(function);
		REQUIRE(testDelegate.Unsubscribe(firstHandle));

		testDelegate.Notify(0.0f);

		REQUIRE(!hasLambdaBeenExecuted); /// \note The given lambda should not be invoked later
	}
}