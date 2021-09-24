/*!
	\file memTracker.hpp
	\date 02.09.2021
	\author Ildar Kasimov

	This file is a single-header library which was written in C++14 standard. It overloads
	global new and delete operators due to control memory allocations. 
	Incompatible with C 

	The usage of the library is pretty simple, just copy this file into your enviornment and
	predefine MEM_TRACKER_IMPLEMENTATION macro before its inclusion like the following

	\code
		#define MEM_TRACKER_IMPLEMENTATION
		#include "memTracker.hpp"
	\endcode

*/

#pragma once


#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstdio>


///< Library's configs
#define MEM_TRACKER_DISABLE_EXCEPTIONS 1
#define MEM_TRACKER_ENABLE_EXPORT 1
#define MEM_TRACKER_REDEFINE_NEW_KEYWORD 0

#define MEM_TRACKER_DEBUG_OUTPUT_STREAM stdout


#if MEM_TRACKER_DISABLE_EXCEPTIONS
#define MEM_TRACKER_NOEXCEPT noexcept
#else
#define MEM_TRACKER_NOEXCEPT 
#endif


#if MEM_TRACKER_ENABLE_EXPORT
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
	typedef struct TMemInfo
	{
		size_t mAllocationsCount = 0;
		size_t mTotalUsedMemory = 0; ///< \note Total allocated memory in bytes excluding sizes of headers

		typedef struct TAllocationInfo
		{
			const char* mpFilename;
			size_t      mLine;
			size_t      mSize;
			uintptr_t   mAddress;

			TAllocationInfo* mpNext;
		} TAllocationInfo, *TAllocationInfoPtr;

		TAllocationInfoPtr mpAllocationsListCurrPtr = nullptr;
	} TMemInfo, *TMemInfoPtr;


	typedef struct TMemAllocationInfo
	{
		WRENCH_API TMemAllocationInfo(const char* pFilename, size_t line);

		const char* mpFilename;
		size_t      mLine;
	} TMemAllocationInfo, *TMemAllocationInfoPtr;


	static TMemInfo MemoryTrackerInfo;


	WRENCH_API const TMemInfo& WRENCH_APIENTRY GetMemoryInfo();


	WRENCH_API void PushMemTrackInfo(TMemInfo::TAllocationInfoPtr& pListHead, const TMemAllocationInfo& info, uintptr_t address, size_t size) MEM_TRACKER_NOEXCEPT;
	WRENCH_API void RemoveMemTrackInfo(uintptr_t address) MEM_TRACKER_NOEXCEPT;


	template <typename T>
	inline T* operator| (const TMemAllocationInfo& info, T* pPtr)
	{
		PushMemTrackInfo(MemoryTrackerInfo.mpAllocationsListCurrPtr, info, reinterpret_cast<uintptr_t>(pPtr), sizeof(T));
		return pPtr;
	}
}


constexpr size_t ALLOCATION_HEADER_SIZE = sizeof(size_t);


#if defined(MEM_TRACKER_IMPLEMENTATION)


namespace Wrench
{
	TMemAllocationInfo::TMemAllocationInfo(const char* pFilename, size_t line) :
		mpFilename(pFilename), mLine(line)
	{
	}


	static TMemInfo::TAllocationInfoPtr CreateMemTrackInfo(const TMemAllocationInfo& info, uintptr_t address, size_t size) MEM_TRACKER_NOEXCEPT
	{
		TMemInfo::TAllocationInfoPtr pNewEntity = reinterpret_cast<TMemInfo::TAllocationInfoPtr>(malloc(sizeof(TMemInfo::TAllocationInfo)));

		pNewEntity->mpNext = nullptr;
		pNewEntity->mAddress = address;
		pNewEntity->mLine = info.mLine;
		pNewEntity->mSize = size;
		pNewEntity->mpFilename = info.mpFilename;

		return pNewEntity;
	}


	void PushMemTrackInfo(TMemInfo::TAllocationInfoPtr& pListHead, const TMemAllocationInfo& info, uintptr_t address, size_t size) MEM_TRACKER_NOEXCEPT
	{
		if (!pListHead) /// \note Create a new node instead of the head, 'cause it's null
		{
			pListHead = CreateMemTrackInfo(info, address, size);
			return;
		}

		TMemInfo::TAllocationInfoPtr pNewEntity = CreateMemTrackInfo(info, address, size);

		pNewEntity->mpNext = pListHead;
		pListHead = pNewEntity;
	}

	void RemoveMemTrackInfo(uintptr_t address) MEM_TRACKER_NOEXCEPT
	{
		TMemInfo::TAllocationInfoPtr pCurrNode = MemoryTrackerInfo.mpAllocationsListCurrPtr;
		TMemInfo::TAllocationInfoPtr pPrevNode = nullptr;

		while (pCurrNode)
		{
			/// \note The node is found
			if (pCurrNode && (pCurrNode->mAddress == address))
			{
				if (pPrevNode)
				{
					pPrevNode->mpNext = pCurrNode->mpNext;
				}
				else
				{
					MemoryTrackerInfo.mpAllocationsListCurrPtr = pCurrNode->mpNext;
				}

				free(pCurrNode);

				return;
			}

			pPrevNode = pCurrNode;
			pCurrNode = pCurrNode->mpNext;
		}
	}


	static inline void* Malloc(size_t size) MEM_TRACKER_NOEXCEPT
	{
		++Wrench::MemoryTrackerInfo.mAllocationsCount;
		Wrench::MemoryTrackerInfo.mTotalUsedMemory += size;

		void* pPtr = malloc(size + ALLOCATION_HEADER_SIZE);

		if (uint8_t* pTypedPtr = static_cast<uint8_t*>(pPtr))
		{
			*reinterpret_cast<size_t*>(pTypedPtr) = size;
			return pTypedPtr + ALLOCATION_HEADER_SIZE;
		}

		WRENCH_UNREACHABLE();

		return nullptr;
	}


	static inline void Free(void* pPtr) MEM_TRACKER_NOEXCEPT
	{
		uint8_t* pTypedPtr = static_cast<uint8_t*>(pPtr) - ALLOCATION_HEADER_SIZE;
		if (!pTypedPtr)
		{
			WRENCH_UNREACHABLE();
			free(pPtr); /// \note Anyway try to free the memory occupied by the pointer

			return;
		}

		--Wrench::MemoryTrackerInfo.mAllocationsCount;
		Wrench::MemoryTrackerInfo.mTotalUsedMemory -= *reinterpret_cast<size_t*>(pTypedPtr);

		RemoveMemTrackInfo(reinterpret_cast<uintptr_t>(pPtr));

		free(pTypedPtr);
	}
}


void* operator new(size_t size) { return Wrench::Malloc(size); }
void* operator new[](size_t size) { return Wrench::Malloc(size); }

void operator delete(void* pPtr) { Wrench::Free(pPtr); }
void operator delete[](void* pPtr) { Wrench::Free(pPtr); }


namespace Wrench
{
#if defined(_MSC_VER)

	static void LogMessage(const char* pMsg)
	{
		fprintf(MEM_TRACKER_DEBUG_OUTPUT_STREAM, pMsg);
	}
#else
	static void LogMessage(const char* pMsg) {}
#endif


	WRENCH_API const TMemInfo& WRENCH_APIENTRY GetMemoryInfo()
	{
		return MemoryTrackerInfo;
	}


	static void PrintMemoryLeaksInformation() MEM_TRACKER_NOEXCEPT
	{
		constexpr size_t maxBufferSize = 512;

		char messageBuffer[maxBufferSize];

		sprintf_s(messageBuffer, maxBufferSize, "Total memory leaks: %d, Memory occupied: %d\n", MemoryTrackerInfo.mAllocationsCount, MemoryTrackerInfo.mTotalUsedMemory);
		LogMessage(messageBuffer);

		TMemInfo::TAllocationInfoPtr pCurrNode = MemoryTrackerInfo.mpAllocationsListCurrPtr;

		while (pCurrNode)
		{
			LogMessage("\n>>>========================================================================\n");

			sprintf_s(messageBuffer, maxBufferSize, "File: %s\nLine:%d\nAddress:%#010x\nSize:%d bytes\n", pCurrNode->mpFilename, pCurrNode->mLine, pCurrNode->mAddress, pCurrNode->mSize);
			LogMessage(messageBuffer);

			pCurrNode = pCurrNode->mpNext;
		}
	}


	static void RemoveDebugMemory() MEM_TRACKER_NOEXCEPT
	{
		TMemInfo::TAllocationInfoPtr pCurrNode = MemoryTrackerInfo.mpAllocationsListCurrPtr;

		while (pCurrNode)
		{
			TMemInfo::TAllocationInfoPtr pTempNode = pCurrNode;
			pCurrNode = pCurrNode->mpNext;

			free(pTempNode);
		}
	}


	/*!
		\brief The object checks up memory leaks when the application is closing
	*/

	typedef struct MemoryLeaksValidator
	{
		~MemoryLeaksValidator()
		{
			PrintMemoryLeaksInformation();
			RemoveDebugMemory();
		}
	} MemoryLeaksValidator;

	static MemoryLeaksValidator validator;
}


#endif

#if MEM_TRACKER_REDEFINE_NEW_KEYWORD
	/// \brief The macro redefines new and should be placed at the end of the header file to prevent collisions
	#define new Wrench::TMemAllocationInfo(__FILE__, __LINE__) | new
#else
	#define WRENCH_NEW Wrench::TMemAllocationInfo(__FILE__, __LINE__) | new
#endif