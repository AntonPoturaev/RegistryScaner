/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include <string>

#include <windows.h>

namespace RegestryScanner { namespace Details {

	std::wstring Handle2Path(HKEY key);

}} /// end namespace RegestryScanner::Details
