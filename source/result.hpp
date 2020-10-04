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


#if RESULT_DISABLE_EXCEPTIONS
	#define RESULT_NOEXCEPT noexcept
#else
	#define RESULT_NOEXCEPT 
#endif

#include <cassert>
#define RESULT_ASSERT(assertion) assert(assertion)



namespace Lib
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
		TOkValue() = delete;
		TOkValue(const T& value): mValue(value) { }
		TOkValue(T&& value): mValue(std::move(value)) { }

		T mValue;
	};

	template <> struct TOkValue<void> {};


	template <typename E>
	struct TErrValue
	{
		TErrValue() = delete;
		TErrValue(const E& value) : mError(value) { }
		TErrValue(E&& value) : mError(std::move(value)) { }

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
			Storage(): mIsInitialized(false), mIsValid(false) { }

			Storage(const TOkValue<T>& value) :
				mIsInitialized(true), mIsValid(true)
			{ 
				new (&mData) T(value.mValue);
			}

			Storage(const TErrValue<E>& value) : 
				mIsInitialized(true), mIsValid(false)
			{
				new (&mData) E(value.mError);
			}

			~Storage()
			{
				_release();
			}

			void Reset(const TOkValue<T>& value)
			{
				if (mIsInitialized)
				{
					_release();
				}

				new (&mData) T(value.mValue);

				mIsInitialized = true;
				mIsValid = true;
			}

			void Reset(const TErrValue<E>& value)
			{
				if (mIsInitialized)
				{
					_release();
				}

				new (&mData) E(value.mError);

				mIsInitialized = true;
			}

			template <typename U>
			const U& GetAs() const
			{
				return *reinterpret_cast<const U*>(&mData);
			}

			template <typename U>
			U& GetAs()
			{
				return *reinterpret_cast<U*>(&mData);
			}

			bool IsValid() const { return mIsValid; }

		private:
			void _release()
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
			Storage() : mIsInitialized(false), mIsValid(false) { }

			Storage(const TOkValue<void>& value) :
				mIsInitialized(true), mIsValid(true)
			{
			}

			Storage(const TErrValue<E>& value) :
				mIsInitialized(true), mIsValid(false)
			{
				new (&mData) E(value.mError);
			}

			~Storage()
			{
				_release();
			}

			void Reset(const TOkValue<void>& value)
			{
				if (mIsInitialized)
				{
					_release();
				}

				mIsInitialized = true;
				mIsValid = true;
			}

			void Reset(const TErrValue<E>& value)
			{
				if (mIsInitialized)
				{
					_release();
				}

				new (&mData) E(value.mError);

				mIsInitialized = true;
			}

			template <typename U>
			const U& GetAs() const
			{
				return *reinterpret_cast<U*>(&mData);
			}

			bool IsValid() const { return mIsValid; }

		private:
			void _release()
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
			Result() = delete;

			Result(const TOkValue<T>& value): mData(value) { }
			Result(const TErrValue<E>& error) : mData(error) { }

			Result(const Result& result) : mData(result.mData) { }
			Result(Result&& result) : mData(std::move(result.mData)) { }

			~Result() = default;

			/// Methods

			template <typename U = T>
			typename std::enable_if<!std::is_same<U, void>::value, U>::type
				Get() const
			{
				if (!mData.IsValid())
				{
					Panic("[Result<T, E>] Get() was invoked for an invalid Result<T, E> object");
				}

				return mData.template GetAs<T>();
			}

			template <typename U = T>
			typename std::enable_if<!std::is_same<U, void>::value, U>::type
				GetOrDefault(U&& altValue)
			{
				if (!mData.IsValid())
				{
					return std::forward<U>(altValue);
				}

				return mData.template GetAs<T>();
			}

			template <typename U = E>
			typename std::enable_if<!std::is_same<U, void>::value, U>::type
				GetError() const
			{
				if (mData.IsValid())
				{
					Panic("[Result<T, E>] GetError() was invoked for a valid Result<T, E> object");
				}

				return mData.GetAs<E>();
			}

			bool IsOk() const { return mData.IsValid(); }
			bool HasError() const { return !mData.IsValid(); }

			/// Operators section

			Result<T, E>& operator= (const TOkValue<T>& value)
			{
				mData.Reset(value);
				return *this;
			}

			Result<T, E>& operator= (const TErrValue<E>& error)
			{
				mData.Reset(error);
				return *this;
			}

			Result<T, E>& operator= (const Result<T, E>& result)
			{
				mData.Reset(result.mData);
				return *this;
			}

			Result<T, E>& operator= (Result<T, E>&& result)
			{
				std::swap(mData, result.mData);
				return *this;
			}

			operator bool() const { return mData.IsValid(); }
		private:
			ResultStorage mData;
	};
}