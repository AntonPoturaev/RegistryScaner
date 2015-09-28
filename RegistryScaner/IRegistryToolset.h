/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "ValueInfo.h"
#include "KeyInfo.h"

namespace RegistryScanner {

	struct IRegistryToolset
	{
		virtual LONG OpenKey(HKEY handle, std::wstring const& subkeyName, DWORD accessMask, PHKEY outHandle) const = 0;
		virtual LONG CloseKey(HKEY handle) const = 0;

		virtual LONG QueryInfoKey(HKEY handle, KeyInfo& keyInfo) const = 0;

		virtual LONG GetKeyNameAtIndex(HKEY handle, DWORD index, std::wstring& ame) const = 0;
		virtual LONG GetValueInfoAtIndex(HKEY handle, DWORD index, ValueInfo& valueInfo) const = 0;
	};

} /// end namespace RegistryScanner
