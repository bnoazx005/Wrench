/*!
	\file deferOperation.hpp
	\date 12.09.2021
	\author Ildar Kasimov

	The library provides 'defer' operator that Go programming language introduces. The closest 
	analogue in C++ is Boost.ScopeExit or some implementations of scope_guard.

	The usage of the library is pretty simple, just copy this file into your enviornment and
	predefine DEFER_IMPLEMENTATION macro before its inclusion like the following

	\code
		#define DEFER_IMPLEMENTATION
		#include "library_template.hpp"
	\endcode

*/

#pragma once


#include <cassert>
#include <functional>


///< Library's configs
#define DEFER_DISABLE_EXCEPTIONS 1
#define DEFER_ENABLE_EXPORT 1
#define DEFER_REDEFINE_NEW_KEYWORD 1

#define DEFER_DEBUG_OUTPUT_STREAM stdout


#if DEFER_DISABLE_EXCEPTIONS
#define DEFER_NOEXCEPT noexcept
#else
#define DEFER_NOEXCEPT 
#endif


#if DEFER_ENABLE_EXPORT
	#if defined(_WIN32) || defined(_MSC_VER)
		#if !defined(WRENCH_APIENTRY)
			#define WRENCH_APIENTRY __cdecl					///< Calling convention for VS
		#endif	

		#if !defined(WRENCH_API)
			#if defined(WRENCH_DLLIMPORT)
				#define WRENCH_API __declspec(dllimport)
			#else
				#define WRENCH_API __declspec(dllexport)
			#endif
		#endif
	#elif defined(__GNUC__)
		#if !defined(WRENCH_APIENTRY)
			#define WRENCH_APIENTRY __attribute__((cdecl))	///< Calling convention for GNUC
		#endif

		#if !defined(WRENCH_API)
			#if defined(WRENCH_DLLIMPORT)
				#define WRENCH_API 
			#else
				#define WRENCH_API __attribute__((visibility("default")))
			#endif
		#endif
			#else /// Unknown platform and compiler
		#define WRENCH_API 
	#endif
#endif


#define WRENCH_ASSERT(condition) assert(condition) 
#define WRENCH_UNREACHABLE() do { assert(false); } while(false)


namespace Wrench
{
	struct TDeferOperation
	{
		typedef std::function<void()> TActionCallback;

		explicit TDeferOperation(TActionCallback action);
		~TDeferOperation();

		TDeferOperation() = delete;
		TDeferOperation(TDeferOperation&) = delete;
		TDeferOperation(TDeferOperation&&) = delete;

		TActionCallback mAction = nullptr;
	};


#define DEFER_CONCAT_ID_IMPL(left, right) left ## right
#define DEFER_CONCAT_ID(left, right) DEFER_CONCAT_ID_IMPL(left, right)
#define DEFER_ID(id) DEFER_CONCAT_ID(id, DEFER_CONCAT_ID(__COUNTER__, __LINE__))


#define defer(op) const TDeferOperation DEFER_ID(deferOp)(op)

#if defined(DEFER_IMPLEMENTATION)

	TDeferOperation::TDeferOperation(std::function<void()> action):
		mAction(action)
	{
	}

	TDeferOperation::~TDeferOperation()
	{
		if (mAction)
		{
			mAction();
		}
	}

#endif

}
