/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "ntdll_module.h"

#include <memory>

namespace RegistryScanner { namespace Details {

	NtDll::NtDll()
		: DllModule(L"ntdll.dll")
		, m_NtQueryKeyProc(nullptr)
	{
	}

	std::wstring NtDll::GetKeyPathFromHKEY(HKEY key)
	{
		std::wstring result;

		if (!m_NtQueryKeyProc && !GetProc("NtQueryKey", m_NtQueryKeyProc))
			return result;

		typedef LONG NTSTATUS;
		
#if !defined(STATUS_SUCCESS)
#	define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif /// !STATUS_SUCCESS

#if !defined(STATUS_BUFFER_TOO_SMALL)
#	define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#endif /// !STATUS_BUFFER_TOO_SMALL

		DWORD size = 0, errCode = 0;
		errCode = m_NtQueryKeyProc(key, 3, 0, 0, &size);
		if (errCode == STATUS_BUFFER_TOO_SMALL)
		{
			size = size + 2;
			std::unique_ptr<wchar_t[]> buffer(new (std::nothrow) wchar_t[size / sizeof(wchar_t)]); /// size is in bytes
			if (buffer)
			{
				errCode = m_NtQueryKeyProc(key, 3, buffer.get(), size, &size);
				if (errCode == STATUS_SUCCESS)
				{
					buffer[size / sizeof(wchar_t)] = L'\0';
					result = std::wstring(buffer.get() + 2);
				}
			}
		}

		return result;
	}

}} /// end namespace RegistryScanner::Details
