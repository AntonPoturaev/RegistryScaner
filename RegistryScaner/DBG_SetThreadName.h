/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma  once

#if !defined(NDEBUG)

#include <windows.h>

namespace {

#pragma pack(push,8)
	typedef struct
	{
		DWORD	dwType;
		LPCSTR	szName;
		DWORD	dwThreadID;
		DWORD	dwFlags;
	}
	THREADNAME_INFO;
#pragma pack(pop)

	inline void _Do_SetThisThreadName(char const* name)
	{
		static DWORD const ExceptionCode = 0x406D1388;
		static DWORD const ExceptionFlags = 0L;

		THREADNAME_INFO info =
		{
			0x1000,	/// Must be 0x1000
			name,	/// Pointer to name (in user addr space).
			-1,		/// Thread ID (-1 = caller thread).
			0		/// Reserved for future use, must be zero.
		};

		DWORD NumberOfArguments = sizeof(info) / sizeof(ULONG_PTR);
		ULONG_PTR* ArgumentsPtr = reinterpret_cast<ULONG_PTR*>(&info);

		__try { 
			RaiseException(ExceptionCode, ExceptionFlags, NumberOfArguments, ArgumentsPtr); 
		} __except (EXCEPTION_EXECUTE_HANDLER) { }
	}

} /// end unnamed namespace

#	define SetThisThreadName(NAME_ARG) _Do_SetThisThreadName(NAME_ARG)

#else

#	define SetThisThreadName(NAME_ARG) ((void)0)

#endif /// !NDEBUG
