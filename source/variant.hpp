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


	template <typename TWhat, typename TCurrent, typename... TArgs>
	struct TFindType
	{
		static constexpr size_t mValue = TFindType<TWhat, TArgs...>::mValue + 1;
	};

	template <typename TWhat, typename TCurrent>
	struct TFindType<TWhat, TCurrent>
	{
		static constexpr size_t mValue = 1;
	};

	template <typename TWhat, typename... TArgs>
	struct TFindType<TWhat, TWhat, TArgs...>
	{
		static constexpr size_t mValue = 0;
	};



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

				mCurrTypeId = TFindType<T, TArgs...>::mValue;

				new (&mStorage) T(value);

				return value;
			}

			template <typename T>
			bool Is() const VARIANT_NOEXCEPT
			{
				return mCurrTypeId == TFindType<T, TArgs...>::mValue;
			}

			template <typename T>
			const T& As() const VARIANT_NOEXCEPT
			{
				return reinterpret_cast<const T&>(mStorage);
			}

			template <typename T>
			T& As() VARIANT_NOEXCEPT
			{
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