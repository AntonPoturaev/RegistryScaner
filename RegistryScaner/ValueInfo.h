/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "Value.h"

#include <string>

namespace RegestryScanner {

	struct ValueInfo
	{
		std::wstring name;
		Value_t value;

		static DWORD const maxValueName = 16383;
	};

	std::wstring ToWideString(ValueInfo const& info);

} /// end namespace RegestryScanner
