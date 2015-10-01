/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "ValueInfo.h"
#include "StringCnv.h"

#include <boost/format.hpp>

namespace RegistryScanner {

	std::wstring ToWideString(ValueInfo const& info)
	{
		return boost::str(boost::wformat(
			L"\tValue info: name: %1%, type: %2%, data: %3%"
			)
			% info.name
			% ValueToType<Value_t>::GetTypeName(info.value)
			% Value2WideString(info.value)
		);
	}

	std::string ToString(ValueInfo const& info)
	{
		return boost::str(boost::format(
			"\tValue info: name: %1%, type: %2%, data: %3%"
			)
			% Details::StringCnv::w2a(info.name)
			% Details::StringCnv::w2a(ValueToType<Value_t>::GetTypeName(info.value))
			% Value2String(info.value)
		);
	}

} /// end namespace RegistryScanner
