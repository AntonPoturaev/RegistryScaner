/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "ScanInfo.h"
#include "Handle2Path.h"

#include <boost/format.hpp>

namespace RegistryScanner {

	namespace {
		struct _DataDecoder 
			: public boost::static_visitor<std::wstring>
		{
			result_type operator()(ValueInfo const& info) const {
				return ToWideString(info);
			}

			result_type operator()(KeyInfo const& info) const {
				return ToWideString(info);
			}
		};
	} /// end unnamed namespace

	std::wstring ToWideString(ScanInfo::Data_t const& infoData) {
		return boost::apply_visitor(_DataDecoder(), infoData);
	}

	std::wstring ToWideString(ScanInfo const& info) 
	{
		static std::wstring const messageFormat(
			L"****************Info for handle:[%1%]****************\n"
			L"\tPath:[%2%]\n"
			L"\tAdditional info: %3%exists\n"
			L"%4%\n\n"
			);

		std::wstring result =
			boost::str(boost::wformat(messageFormat)
			% info.handle
			% Details::Handle2Path(info.handle)
			% (info.data ? L"" : L"not ")
			% (info.data ? ToWideString(info.data.get()) : L"")
			);

		return result;
	}

} /// end namespace RegistryScanner
