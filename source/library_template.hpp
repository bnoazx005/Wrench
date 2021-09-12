/*!
	\file library_template.hpp
	\date DD.MM.YYYY
	\author Your name

	A brief description of the library

	The usage of the library is pretty simple, just copy this file into your enviornment and
	predefine LIBRARY_TEMPLATE_IMPLEMENTATION macro before its inclusion like the following

	\code
		#define LIBRARY_TEMPLATE_IMPLEMENTATION
		#include "library_template.hpp"
	\endcode

*/

#pragma once


#include <cassert>


///< Library's configs
#define LIBRARY_TEMPLATE_DISABLE_EXCEPTIONS 1
#define LIBRARY_TEMPLATE_ENABLE_EXPORT 1
#define LIBRARY_TEMPLATE_REDEFINE_NEW_KEYWORD 1

#define LIBRARY_TEMPLATE_DEBUG_OUTPUT_STREAM stdout


#if LIBRARY_TEMPLATE_DISABLE_EXCEPTIONS
#define LIBRARY_TEMPLATE_NOEXCEPT noexcept
#else
#define LIBRARY_TEMPLATE_NOEXCEPT 
#endif


#if LIBRARY_TEMPLATE_ENABLE_EXPORT
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

	/// \note Put all declarations here

#if defined(LIBRARY_TEMPLATE_IMPLEMENTATION)

	/// \note Consider this section as *.cpp file

#endif

}
