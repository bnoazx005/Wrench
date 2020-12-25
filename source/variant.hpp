/*!
	\file variant.hpp
	\date 24.12.2020
	\author Ildar Kasimov

	This file is a single-header library which was written in C++14 standard.
	The library provides an implementation of a sigma type which is also known as a tagged union or a variant.

	The library has almost zero dependencies except a few standard library's containers.

*/

#pragma once


#include <algorithm>
#include <cassert>


///< Library's configs
#define VARIANT_DISABLE_EXCEPTIONS 1
#define VARIANT_ENABLE_EXPORT 1


#if VARIANT_DISABLE_EXCEPTIONS
#define VARIANT_NOEXCEPT noexcept
#else
#define VARIANT_NOEXCEPT 
#endif


namespace Wrench
{
	/*!
		\brief Utilities

		If you don't want to be scared just skip this horrible meta-programming magic stuffs
	*/

	template <typename... TArgs> 
	struct TMaxSize
	{
		static constexpr size_t mValue = 0;
	};

	template <typename T, typename... TArgs> 
	struct TMaxSize<T, TArgs...>
	{
		static constexpr size_t mValue = std::max<size_t>(sizeof(T), TMaxSize<TArgs...>::mValue);
	};


	template <typename... TArgs>
	struct TMaxAlignOf
	{
		static constexpr size_t mValue = 0;
	};

	template <typename T, typename... TArgs>
	struct TMaxAlignOf<T, TArgs...>
	{
		static constexpr size_t mValue = std::max<size_t>(alignof(T), TMaxSize<TArgs...>::mValue);
	};

	
	template <size_t, typename>	constexpr size_t GetIndexOfTypeInternal() { return (std::numeric_limits<size_t>::max)(); } // nothing found

	template <size_t index, typename TWhat, typename TCurrent, typename... TArgs>
	constexpr size_t GetIndexOfTypeInternal()
	{
		return std::is_same<TWhat, TCurrent>::value ? index : GetIndexOfTypeInternal<index + 1, TWhat, TArgs...>();
	}

	template <typename TWhat, typename... TArgs> constexpr size_t GetIndexOfType() { return GetIndexOfTypeInternal<0, TWhat, TArgs...>(); }


	/*!
		class Variant

		\brief An implementation of a sigma type which is also known as a tagged union or a variant
	*/

	template <typename... TArgs>
	class Variant
	{
		public:
			using TStorageType = typename std::aligned_storage<TMaxSize<TArgs...>::mValue>::type;
			using TTypeIndex = size_t;
		public:
			Variant() VARIANT_NOEXCEPT :
				mStorage(), mCurrTypeId(0)
			{
			}

			Variant(const Variant& ref) VARIANT_NOEXCEPT :
				mStorage(ref.mStorage), mCurrTypeId(ref.mCurrTypeId)
			{
			}

			Variant(Variant&& ref) VARIANT_NOEXCEPT :
				mStorage(std::move(ref.mStorage)), mCurrTypeId(ref.mCurrTypeId)
			{
			}

			~Variant() VARIANT_NOEXCEPT
			{
				mStorage.~TStorageType();
			}

			// Assignment operator
			template <typename T>
			const T& operator= (const T& value) VARIANT_NOEXCEPT
			{
				if (mCurrTypeId)
				{
					mStorage.~TStorageType();
				}

				mCurrTypeId = GetIndexOfType<T, TArgs...>();

				new (&mStorage) T(value);

				return value;
			}

			template <typename T>
			bool Is() const VARIANT_NOEXCEPT
			{
				return mCurrTypeId == GetIndexOfType<T, TArgs...>();
			}

			template <typename T>
			const T& As() const VARIANT_NOEXCEPT
			{
				if (!Is<T>())
				{
					assert(false);
					std::terminate();
				}

				return reinterpret_cast<const T&>(mStorage);
			}

			template <typename T>
			T& As() VARIANT_NOEXCEPT
			{
				if (!Is<T>())
				{
					assert(false);
					std::terminate();
				}

				return reinterpret_cast<T&>(mStorage);
			}
		private:
			TStorageType mStorage;
			TTypeIndex mCurrTypeId = 0x0;
	};


	template <typename T, typename... TArgs> 
	Variant<TArgs...> MakeVariant(T value)
	{
		Variant<TArgs...> v; v = std::forward<T>(value);
		return std::move(v);
	}
}