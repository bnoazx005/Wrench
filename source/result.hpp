/*!
	\file result.hpp
	\date 03.10.2020
	\author Ildar Kasimov

	This file is a single-header library which was written in C++14 standard.
	The library is an implementation of a type to provide simple yet explicit 
	error handling like that's implemented in Rust programming language.

	The usage of the library is pretty simple, just copy this file into your enviornment and
	predefine RESULT_IMPLEMENTATION macro before its inclusion like the following
	
	\code 
		#define RESULT_IMPLEMENTATION
		#include "result.hpp"
	\endcode

	The library has almost zero dependencies except a few standard library's containers.

*/

#pragma once


#include <type_traits>
#include <iostream>
#include <string>


///< Library's configs
#define RESULT_DISABLE_EXCEPTIONS 1
#define RESULT_ENABLE_EXPORT 1


#if RESULT_DISABLE_EXCEPTIONS
	#define RESULT_NOEXCEPT noexcept
#else
	#define RESULT_NOEXCEPT 
#endif


namespace Wrench
{
	/*!
		\brief The function stops execution of a program
		The method is used to terminate a program when some error
		occurs instead of trying to solve it

		\param[in] message A message that will be printed as an error output
	*/

	inline void Panic(const std::string& message) RESULT_NOEXCEPT
	{
		std::cerr << message << std::endl;
		std::terminate();
	}


	template <typename T>
	struct TOkValue
	{
		TOkValue() RESULT_NOEXCEPT = delete;
		TOkValue(const T& value) RESULT_NOEXCEPT : mValue(value) { }
		TOkValue(T&& value) RESULT_NOEXCEPT : mValue(std::move(value)) { }

		T mValue;
	};

	template <> struct TOkValue<void> {};


	template <typename E>
	struct TErrValue
	{
		TErrValue() RESULT_NOEXCEPT = delete;
		TErrValue(const E& value) RESULT_NOEXCEPT : mError(value) { }
		TErrValue(E&& value) RESULT_NOEXCEPT : mError(std::move(value)) { }

		E mError;
	};


	template <typename T, typename E>
	class Storage final
	{
		static_assert(!std::is_same<E, void>::value, "[Storage<T, E>] \"void\" is not allowed as an error type E");

		public:
			static constexpr size_t mSize = (sizeof(T) > sizeof(E)) ? sizeof(T) : sizeof(E);
			static constexpr size_t mAlignment = (alignof(T) > alignof(E)) ? alignof(E) : alignof(E);

			using StorageImpl = typename std::aligned_storage<mSize, mAlignment>::type;
		public:
			Storage() RESULT_NOEXCEPT : mIsInitialized(false), mIsValid(false) { }

			Storage(const TOkValue<T>& value) RESULT_NOEXCEPT :
				mIsInitialized(true), mIsValid(true)
			{ 
				new (&mData) T(value.mValue);
			}

			Storage(const TErrValue<E>& value) RESULT_NOEXCEPT :
				mIsInitialized(true), mIsValid(false)
			{
				new (&mData) E(value.mError);
			}

			~Storage() RESULT_NOEXCEPT
			{
				_release();
			}

			void Reset(const TOkValue<T>& value) RESULT_NOEXCEPT
			{
				if (mIsInitialized)
				{
					_release();
				}

				new (&mData) T(value.mValue);

				mIsInitialized = true;
				mIsValid = true;
			}

			void Reset(const TErrValue<E>& value) RESULT_NOEXCEPT
			{
				if (mIsInitialized)
				{
					_release();
				}

				new (&mData) E(value.mError);

				mIsInitialized = true;
			}

			template <typename U>
			const U& GetAs() const RESULT_NOEXCEPT
			{
				return *reinterpret_cast<const U*>(&mData);
			}

			template <typename U>
			U& GetAs() RESULT_NOEXCEPT
			{
				return *reinterpret_cast<U*>(&mData);
			}

			bool IsValid() const RESULT_NOEXCEPT { return mIsValid; }

		private:
			void _release() RESULT_NOEXCEPT
			{
				if (mIsInitialized)
				{
					if (mIsValid)
					{
						GetAs<T>().~T();
					}
					else
					{
						GetAs<E>().~E();
					}
				}

				mIsInitialized = false;
				mIsValid = false;
			}

		private:
			bool mIsInitialized : 2;
			bool mIsValid : 2;

			StorageImpl mData;
	};


	template <typename E>
	class Storage<void, E> final
	{
		static_assert(!std::is_same<E, void>::value, "[Storage<T, E>] \"void\" is not allowed as an error type E");

		public:
			using StorageImpl = typename std::aligned_storage<sizeof(E), alignof(E)>::type;

		public:
			Storage() RESULT_NOEXCEPT : mIsInitialized(false), mIsValid(false) { }

			Storage(const TOkValue<void>& value) RESULT_NOEXCEPT :
				mIsInitialized(true), mIsValid(true)
			{
			}

			Storage(const TErrValue<E>& value) RESULT_NOEXCEPT :
				mIsInitialized(true), mIsValid(false)
			{
				new (&mData) E(value.mError);
			}

			~Storage() RESULT_NOEXCEPT
			{
				_release();
			}

			void Reset(const TOkValue<void>& value) RESULT_NOEXCEPT
			{
				if (mIsInitialized)
				{
					_release();
				}

				mIsInitialized = true;
				mIsValid = true;
			}

			void Reset(const TErrValue<E>& value) RESULT_NOEXCEPT
			{
				if (mIsInitialized)
				{
					_release();
				}

				new (&mData) E(value.mError);

				mIsInitialized = true;
			}

			template <typename U>
			const U& GetAs() const RESULT_NOEXCEPT
			{
				return *reinterpret_cast<U*>(&mData);
			}

			bool IsValid() const RESULT_NOEXCEPT { return mIsValid; }

		private:
			void _release() RESULT_NOEXCEPT
			{
				if (mIsInitialized)
				{
					if (!mIsValid)
					{
						GetAs<E>().~E();
					}
				}

				mIsInitialized = false;
				mIsValid = false;
			}

		private:
			bool mIsInitialized : 2;
			bool mIsValid : 2;

			StorageImpl mData;
	};


	template <typename T, typename E>
	class Result final
	{
		static_assert(!std::is_same<E, void>::value, "[Result<T, E>] \"void\" is not allowed as an error type E");

		public:
			using ResultStorage = Storage<T, E>;

		public:
			Result() RESULT_NOEXCEPT = delete;

			Result(const TOkValue<T>& value) RESULT_NOEXCEPT : mData(value) { }
			Result(const TErrValue<E>& error) RESULT_NOEXCEPT : mData(error) { }

			Result(const Result& result) RESULT_NOEXCEPT : mData(result.mData) { }
			Result(Result&& result) RESULT_NOEXCEPT : mData(std::move(result.mData)) { }

			~Result() RESULT_NOEXCEPT = default;

			/// Methods

			template <typename U = T>
			typename std::enable_if<!std::is_same<U, void>::value, U>::type
				Get() const RESULT_NOEXCEPT
			{
				if (!mData.IsValid())
				{
					Panic("[Result<T, E>] Get() was invoked for an invalid Result<T, E> object");
				}

				return mData.template GetAs<U>();
			}

			template <typename U = T>
			typename std::enable_if<!std::is_same<U, void>::value, U>::type
				GetOrDefault(U&& altValue) RESULT_NOEXCEPT
			{
				if (!mData.IsValid())
				{
					return std::forward<U>(altValue);
				}

				return mData.template GetAs<U>();
			}

			template <typename U = E>
			typename std::enable_if<!std::is_same<U, void>::value, U>::type
				GetError() const RESULT_NOEXCEPT
			{
				if (mData.IsValid())
				{
					Panic("[Result<T, E>] GetError() was invoked for a valid Result<T, E> object");
				}

				return mData.template GetAs<U>();
			}

			bool IsOk() const RESULT_NOEXCEPT { return mData.IsValid(); }
			bool HasError() const RESULT_NOEXCEPT { return !mData.IsValid(); }

			/// Operators section

			Result<T, E>& operator= (const TOkValue<T>& value) RESULT_NOEXCEPT
			{
				mData.Reset(value);
				return *this;
			}

			Result<T, E>& operator= (const TErrValue<E>& error) RESULT_NOEXCEPT
			{
				mData.Reset(error);
				return *this;
			}

			Result<T, E>& operator= (const Result<T, E>& result) RESULT_NOEXCEPT
			{
				mData.Reset(result.mData);
				return *this;
			}

			Result<T, E>& operator= (Result<T, E>&& result) RESULT_NOEXCEPT
			{
				std::swap(mData, result.mData);
				return *this;
			}

			operator bool() const RESULT_NOEXCEPT { return mData.IsValid(); }
		private:
			ResultStorage mData;
	};
}