/*!
	\file delegate.hpp
	\date 26.11.2021
	\author Ildar Kasimov

	The library implements simple events listening system like those that exist in C# language. In C++ this concept is called
	'signals-slots' but both are kinds of observer pattern.

	The usage of the library is pretty simple, just copy this file into your enviornment and
	predefine DELEGATE_IMPLEMENTATION macro before its inclusion like the following

	\code
		#define DELEGATE_IMPLEMENTATION
		#include "delegate.hpp"
	\endcode

*/

#pragma once


#include <cassert>
#include <functional>
#include <vector>
#include <algorithm>
#include <limits>


///< Library's configs
#define DELEGATE_DISABLE_EXCEPTIONS 1
#define DELEGATE_ENABLE_EXPORT 1
#define DELEGATE_ENABLE_THREAD_SAFENESS 1


#if DELEGATE_DISABLE_EXCEPTIONS
	#define DELEGATE_NOEXCEPT noexcept
#else
	#define DELEGATE_NOEXCEPT 
#endif


#if DELEGATE_ENABLE_EXPORT
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

#if DELEGATE_ENABLE_THREAD_SAFENESS
	#include <mutex>

	#define DELEGATE_LOCK_THREAD std::lock_guard<std::mutex> lock(mMutex)
	#define DELEGATE_DECLARE_OBJECT_MUTEX mutable std::mutex mMutex
#else
	#define DELEGATE_LOCK_THREAD
	#define DELEGATE_DECLARE_OBJECT_MUTEX
#endif


namespace Wrench
{
	enum class TSubscriptionHandle : uint32_t { Invalid = (std::numeric_limits<uint32_t>::max)() };


	template <typename... TArgs>
	class Delegate final
	{
		public:
			Delegate() DELEGATE_NOEXCEPT 
				: mFirstFreeEntityIndex((std::numeric_limits<size_t>::max)()) {}
			~Delegate() DELEGATE_NOEXCEPT { UnsubscribeAll(); }

			TSubscriptionHandle Subscribe(std::function<void(TArgs...)> listener) DELEGATE_NOEXCEPT
			{
				DELEGATE_LOCK_THREAD;

				if (!listener)
				{
					return TSubscriptionHandle::Invalid;
				}

#if 0 /// \todo
				auto it = std::find_if(mListeners.cbegin(), mListeners.cend(), [&listener](auto&& currEntity)
				{
					return reinterpret_cast<uintptr_t>(currEntity.target()) == reinterpret_cast<uintptr_t>(listener.target());
				});
				if (it != mListeners.cend())
				{
					return static_cast<TSubscriptionHandle>(std::distance(mListeners.cbegin(), it)); /// \note The listener's already been registered so return its handle
				}
#endif

				if (mFirstFreeEntityIndex < mListeners.size())
				{
					mListeners[mFirstFreeEntityIndex] = std::move(listener);
					auto result = static_cast<TSubscriptionHandle>(mFirstFreeEntityIndex);

					mFirstFreeEntityIndex = mListeners.size();

					return result;
				}

				mListeners.emplace_back(listener);

				return static_cast<TSubscriptionHandle>(mListeners.size() - 1);
			}

			bool Unsubscribe(TSubscriptionHandle subscriptionHandle) DELEGATE_NOEXCEPT
			{
				DELEGATE_LOCK_THREAD;

				size_t index = static_cast<size_t>(subscriptionHandle);
				if (index >= mListeners.size())
				{
					WRENCH_ASSERT(false);
					return false;
				}

				if (!mListeners[index])
				{
					return false;
				}

				mListeners[index] = nullptr;
				mFirstFreeEntityIndex = static_cast<size_t>(subscriptionHandle);

				return true;
			}

			void UnsubscribeAll() DELEGATE_NOEXCEPT
			{
				DELEGATE_LOCK_THREAD;

				mFirstFreeEntityIndex = (std::numeric_limits<size_t>::max)();
				mListeners.clear(); 
			}

			void Notify(TArgs&&... args) DELEGATE_NOEXCEPT
			{
				DELEGATE_LOCK_THREAD;

				for (auto&& currListener : mListeners)
				{
					if (currListener)
					{
						currListener(std::forward<TArgs>(args)...);
					}
				}
			}

			void operator()(TArgs&&... args) DELEGATE_NOEXCEPT { Notify(std::forward<TArgs>(args)...); }

			TSubscriptionHandle operator+= (std::function<void(TArgs...)> listener) DELEGATE_NOEXCEPT { return Subscribe(listener); }
			bool operator-= (TSubscriptionHandle handle) DELEGATE_NOEXCEPT { return Unsubscribe(handle); }
		private:
			std::vector<std::function<void(TArgs...)>> mListeners;
			size_t mFirstFreeEntityIndex = 0;

			DELEGATE_DECLARE_OBJECT_MUTEX;
	};

}
