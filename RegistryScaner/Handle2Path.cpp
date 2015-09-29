/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "Handle2Path.h"
#include "SimpleSingletonTpl.h"
#include "ntdll.h"

namespace RegistryScanner { namespace Details {

	std::wstring Handle2Path(HKEY key)
	{
		std::wstring result;

		if (key == nullptr)
			result = L"Bad registry handle(is null)!";
		else if (key == HKEY_CURRENT_USER)
			result = L"HKEY_CURRENT_USER";
		else if (key == HKEY_LOCAL_MACHINE)
			result = L"HKEY_LOCAL_MACHINE";
		else if (key == HKEY_USERS)
			result = L"HKEY_USERS";
		else if (key == HKEY_CLASSES_ROOT)
			result = L"HKEY_CLASSES_ROOT";
		else if (key == HKEY_CURRENT_CONFIG)
			result = L"HKEY_CURRENT_CONFIG";
		else
			result = SimpleSingleton<NtDll>::Instance().GetKeyPathFromHKEY(key);

		return result;
	}

}} /// end namespace RegistryScanner::Details
