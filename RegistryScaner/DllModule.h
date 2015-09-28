/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include <winbase.h>
#include <xstring>
#include <assert.h>

namespace RegistryScanner { namespace Details {

	class DllModule
	{
	public:
		typedef std::basic_string<TCHAR> String;

	public:
		explicit DllModule(String const& libName, DWORD flags = 0);
		virtual ~DllModule();

		DllModule(DllModule const&);
		DllModule& operator= (DllModule const&);

		bool Load(String const& libName, DWORD flags = 0);
		void Free();

		bool IsLoad() const;

		template<typename PROCADDR>
		bool GetProc(LPCSTR procName, PROCADDR& addr)
		{
			if (IsLoad())
			{
				addr = reinterpret_cast<PROCADDR>(::GetProcAddress(m_Handle, procName));

				assert(addr);
				if (!addr)
					return false;

				return true;
			}

			return false;
		}

	protected:
		String m_LibName;
		DWORD m_Flags;
		HMODULE m_Handle;
	};

}} /// end namespace RegistryScanner::Details
