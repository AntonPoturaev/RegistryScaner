/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "RegistryToolsetBase.h"

namespace RegestryScanner {

	class HiLevelRegistryToolset 
		: public RegistryToolsetBase
	{
	public:
		virtual LONG OpenKey(HKEY handle, std::wstring const& subkeyName, DWORD accessMask, PHKEY outHandle) const override;
		virtual LONG CloseKey(HKEY handle) const override;

		virtual LONG QueryInfoKey(HKEY handle, KeyInfo& keyInfo) const override;
		virtual LONG GetKeyNameAtIndex(HKEY handle, DWORD index, std::wstring& name) const override;
		virtual LONG GetValueInfoAtIndex(HKEY handle, DWORD index, ValueInfo& valueInfo) const override;
	};

} /// end namespace RegestryScanner
