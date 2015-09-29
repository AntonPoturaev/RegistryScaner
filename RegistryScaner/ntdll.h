/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "Dll.h"

namespace RegistryScanner { namespace Details {

	class NtDll 
		: public Dll
	{
		typedef DWORD(__stdcall *NtQueryKeyProc_t)(
			HANDLE  keyHandle, int keyInformationClass, PVOID keyInformation, ULONG length, PULONG resultLength);
	public:
		NtDll();

		std::wstring GetKeyPathFromHKEY(HKEY key);

	private:
		NtQueryKeyProc_t m_NtQueryKeyProc;
	};

}} /// end namespace RegistryScanner::Details
