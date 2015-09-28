/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "KeyInfo.h"

#include <boost/format.hpp>

namespace RegestryScanner {

	KeyInfo::KeyInfo() {
		::memset(this, 0, sizeof(KeyInfo));
	}

	std::wstring ToWideString(KeyInfo const& info)
	{
		return boost::str(boost::wformat(L"\tKey info: name: %1%, sub key count: %2%, value count: %3%")
			% info.classNameBuffer.ToString()
			% info.subKeysCount
			% info.valuesForKeyCount
		);
	}

} /// end namespace RegestryScanner
