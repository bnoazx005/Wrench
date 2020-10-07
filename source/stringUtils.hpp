/*!
	\file stringUtils.hpp
	\date 05.10.2020
	\author Ildar Kasimov

	This file is a single-header library which was written in C++14 standard.
	The library is a bunch of helpers for std::string type.

	The usage of the library is pretty simple, just copy this file into your enviornment and
	predefine STR_UTILS_IMPLEMENTATION macro before its inclusion like the following
	
	\code 
		#define STR_UTILS_IMPLEMENTATION
		#include "stringUtils.hpp"
	\endcode

*/

#pragma once


#include <type_traits>
#include <string>
#include <cctype>
#include <sstream>


///< Library's configs
#define STR_UTILS_DISABLE_EXCEPTIONS 1


#if STR_UTILS_DISABLE_EXCEPTIONS
#define STR_UTILS_NOEXCEPT noexcept
#else
#define STR_UTILS_NOEXCEPT 
#endif


namespace Wrench
{
	class StringUtils
	{
		public:
			/*!
				\brief The method removes all extra whitespaces from a given string

				\param[in] str A processing string

				\return A processed string where a continuous sequence of whitespaces is replaced with a single one
			*/

			static std::string RemoveExtraWhitespaces(const std::string& str) STR_UTILS_NOEXCEPT;

			/*!
				\brief The method remove all whitespaces from a given string

				\param[in] str A processing string

				\return A processed string without spaces
			*/

			static std::string RemoveAllWhitespaces(const std::string& str) STR_UTILS_NOEXCEPT;

			/*!
				\brief The method splits a given string into an array of substring which are separated with given delimiters

				\param[in] str An original string

				\param[in] delims A string contains delimiters

				\return An array of substring that are separated with delimiters
			*/

			static std::vector<std::string> Split(const std::string& str, const std::string& delims) STR_UTILS_NOEXCEPT;

			/*!
				\brief The method returns an empty string

				\return The method returns an empty string
			*/

			static const std::string& GetEmptyStr() STR_UTILS_NOEXCEPT;

			/*!
				\brief The method checks up whether the given string stars from specified prefix string or not

				\return Returns true if first parameter contains the second as a prefix
			*/

			static bool StartsWith(const std::string& str, const std::string& prefix) STR_UTILS_NOEXCEPT;

			template <typename... TArgs>
			static std::string Format(const std::string& formatStr, TArgs&&... args) STR_UTILS_NOEXCEPT
			{
				constexpr size_t argsCount = sizeof...(args);

				std::array<std::string, argsCount> arguments;
				_convertToStringsArray<sizeof...(args), TArgs...>(arguments, std::forward<TArgs>(args)...);

				std::string formattedStr = formatStr;
				std::string currArgValue;
				std::string currArgPattern;

				currArgPattern.reserve(5);

				std::string::size_type pos = 0;

				/// \note replace the following patterns {i}
				for (size_t i = 0; i < argsCount; ++i)
				{
					currArgPattern = "{" + ToString<size_t>(i) + "}";
					currArgValue = arguments[i];

					while ((pos = formattedStr.find(currArgPattern)) != std::string::npos)
					{
						formattedStr.replace(pos, currArgPattern.length(), currArgValue);
					}
				}

				return formattedStr;
			}

			template <typename T>
			static std::string ToString(const T& arg) STR_UTILS_NOEXCEPT
			{
				std::ostringstream stream;
				stream << arg;
				return stream.str();
			}
		public:
			static const std::string mEmptyStr;
		private:
			template <uint32_t size>
			static void _convertToStringsArray(std::array<std::string, size>& outArray) STR_UTILS_NOEXCEPT {}

			template <uint32_t size, typename Head, typename... Tail>
			static void _convertToStringsArray(std::array<std::string, size>& outArray, Head&& firstArg, Tail&&... rest) STR_UTILS_NOEXCEPT
			{
				outArray[size - 1 - sizeof...(Tail)] = ToString(std::forward<Head>(firstArg));
				_convertToStringsArray<size, Tail...>(outArray, std::forward<Tail>(rest)...);
			}
	};


#if defined(STR_UTILS_IMPLEMENTATION)


	const std::string StringUtils::mEmptyStr {};


	std::string StringUtils::RemoveExtraWhitespaces(const std::string& str) STR_UTILS_NOEXCEPT
	{
		bool isPrevChSpace = false;

		std::string processedStr{ str };

		processedStr.erase(std::remove_if(processedStr.begin(), processedStr.end(), [&isPrevChSpace](char ch)
		{
			bool isCurrChSpace = std::isspace(ch);

			bool result = isCurrChSpace && isPrevChSpace;

			isPrevChSpace = isCurrChSpace;

			return result;
		}), processedStr.end());

		return processedStr;
	}

	std::string StringUtils::RemoveAllWhitespaces(const std::string& str) STR_UTILS_NOEXCEPT
	{
		std::string processedStr{ str };

		processedStr.erase(std::remove_if(processedStr.begin(), processedStr.end(), [](char ch)
		{
			return std::isspace(ch);
		}), processedStr.end());

		return processedStr;
	}

	std::vector<std::string> StringUtils::Split(const std::string& str, const std::string& delims) STR_UTILS_NOEXCEPT
	{
		std::string::size_type pos = 0;
		std::string::size_type currPos = 0;

		std::string currToken;

		std::vector<std::string> tokens;

		while ((currPos < str.length()) && ((pos = str.find_first_of(delims, currPos)) != std::string::npos))
		{
			currToken = std::move(str.substr(currPos, pos - currPos));

			if (!currToken.empty())
			{
				tokens.emplace_back(std::move(currToken));
			}

			currPos = pos + 1;
		}

		/// insert last token if it wasn't pushed back before
		if (currPos != str.length())
		{
			tokens.emplace_back(std::move(str.substr(currPos, str.length() - currPos)));
		}

		return std::move(tokens);
	}

	const std::string& StringUtils::GetEmptyStr() STR_UTILS_NOEXCEPT
	{
		return mEmptyStr;
	}

	bool StringUtils::StartsWith(const std::string& str, const std::string& prefix) STR_UTILS_NOEXCEPT
	{
		return str.rfind(prefix, 0) == 0;
	}


#endif
}