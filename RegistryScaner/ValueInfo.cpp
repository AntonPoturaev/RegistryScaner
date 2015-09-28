/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "ValueInfo.h"

#include <boost/format.hpp>

namespace RegistryScanner {

	std::wstring ToWideString(ValueInfo const& info)
	{
		return boost::str(boost::wformat(
			L"\tValue info: name: %1%, type: %2%, data: %3%"
			//L"\tValue info: name: %1%, type: %2%"
			)
			% info.name
			% ValueToType<Value_t>::GetTypeName(info.value)
			% Value2WideString(info.value)
		);
	}

} /// end namespace RegistryScanner
