/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include <windows.h>
#include <string>

namespace RegestryScanner { namespace Details {

	template<DWORD Capacity>
	struct WideStaticBuffer
	{
		WideStaticBuffer() {
			::memset(this, 0, sizeof(WideStaticBuffer));
		}

		std::wstring ToString() const {
			return std::wstring(data, size);
		}

		TCHAR data[Capacity];
		DWORD size;
	};

}} /// end namespace RegestryScanner::Details
