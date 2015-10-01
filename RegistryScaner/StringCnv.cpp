/**
Project - Registry Scanner
Copyright (c) 2015 Poturaev A.S.
e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "StringCnv.h"

#include <locale>
#include <codecvt>


namespace RegistryScanner { namespace Details {

	std::wstring StringCnv::a2w(std::string const& str) {
		return a2w(str.c_str());
	}

	std::wstring StringCnv::a2w(char const* str)
	{
		try {
			return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(str);
		} catch (...) {
			return L"*** Failed to call a2w";
		}
	}
	
	std::string StringCnv::w2a(wchar_t const* str)
	{
		try {
			return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
		} catch (...) {
			return "*** Failed to call w2a";
		}
	}

	std::string StringCnv::w2a(std::wstring const& str) {
		return w2a(str.c_str());
	}


}} /// end namespace RegistryScanner::Details
