/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include <xstring>

namespace RegistryScanner { namespace Details {

	struct StringCnv
	{
		static std::wstring a2w(std::string const& str);
		static std::wstring a2w(char const* str);

		static std::string w2a(wchar_t const* str);
		static std::string w2a(std::wstring const& str);
	};

}} /// end namespace RegistryScanner::Details
