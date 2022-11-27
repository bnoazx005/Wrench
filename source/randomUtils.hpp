/*!
	\file randomUtils.hpp
	\date 26.11.2022
	\author Ildar Kasimov

	This file is a single-header library which was written in C++14 standard.
	The library is a bunch of helpers for built-in C++ random types.

	The usage of the library is pretty simple, just copy this file into your enviornment and
	predefine RAND_UTILS_IMPLEMENTATION macro before its inclusion like the following
	
	\code 
		#define RAND_UTILS_IMPLEMENTATION
		#include "randomUtils.hpp"
	\endcode
*/

#pragma once


#include <cassert>
#include <random>


///< Library's configs
#define RAND_UTILS_DISABLE_EXCEPTIONS 1
#define RAND_UTILS_ENABLE_EXPORT 1
#define RAND_UTILS_REDEFINE_NEW_KEYWORD 1

#define RAND_UTILS_DEBUG_OUTPUT_STREAM stdout


#if RAND_UTILS_DISABLE_EXCEPTIONS
#define RAND_UTILS_NOEXCEPT noexcept
#else
#define RAND_UTILS_NOEXCEPT 
#endif


#if RAND_UTILS_ENABLE_EXPORT
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
	/*!
		\brief The class encapsulates uniformly distributed random generator for both integers and floating point ones
	*/

	template <typename TIntegerType = int, typename TRealType = float>
	class Random
	{
		public:
			WRENCH_API explicit Random(TIntegerType seed) RAND_UTILS_NOEXCEPT:
				mSeed(seed),
				mRandomEngine(seed)
			{
			}

			WRENCH_API TIntegerType Get(TIntegerType from = (std::numeric_limits<TIntegerType>::min)(), TIntegerType to = (std::numeric_limits<TIntegerType>::max)()) RAND_UTILS_NOEXCEPT
			{
				using TParamType = typename decltype(mUniformIntDistr)::param_type;
				return mUniformIntDistr(mRandomEngine, TParamType(from, to));
			}

			WRENCH_API TRealType Get(TRealType from = (std::numeric_limits<TRealType>::min)(), TRealType to = (std::numeric_limits<TRealType>::max)()) RAND_UTILS_NOEXCEPT
			{
				using TParamType = typename decltype(mUniformFloatDistr)::param_type;
				return mUniformFloatDistr(mRandomEngine, TParamType(from, to));
			}
		private:
			WRENCH_API Random(const Random&) = delete ;
			WRENCH_API Random& operator=(const Random&) = delete;
		private:
			std::default_random_engine mRandomEngine;

			std::uniform_int_distribution<TIntegerType> mUniformIntDistr;
			std::uniform_real_distribution<TRealType> mUniformFloatDistr;
			
			TIntegerType mSeed = 0;
	};


	using DefaultRandom = Random<int, float>;


#if defined(RAND_UTILS_IMPLEMENTATION)

#endif

}
