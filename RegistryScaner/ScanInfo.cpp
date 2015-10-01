/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "ScanInfo.h"
#include "Handle2Path.h"
#include "StringCnv.h"

#include <boost/format.hpp>

namespace RegistryScanner {

	namespace {
		template<typename StringT>
		struct _DataDecoder;

		template<>
		struct _DataDecoder<std::wstring>
			: public boost::static_visitor<std::wstring>
		{
			result_type operator()(ValueInfo const& info) const {
				return ToWideString(info);
			}

			result_type operator()(KeyInfo const& info) const {
				return ToWideString(info);
			}
		};

		template<>
		struct _DataDecoder<std::string>
			: public boost::static_visitor<std::string>
		{
			result_type operator()(ValueInfo const& info) const {
				return ToString(info);
			}

			result_type operator()(KeyInfo const& info) const {
				return ToString(info);
			}
		};
	} /// end unnamed namespace

	std::wstring ToWideString(ScanInfo::Data_t const& infoData) {
		return boost::apply_visitor(_DataDecoder<std::wstring>(), infoData);
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

	std::string ToString(ScanInfo::Data_t const& infoData) {
		return boost::apply_visitor(_DataDecoder<std::string>(), infoData);
	}

	std::string ToString(ScanInfo const& info)
	{
		static std::string const messageFormat(
			"****************Info for handle:[%1%]****************\n"
			"\tPath:[%2%]\n"
			"\tAdditional info: %3%exists\n"
			"%4%\n\n"
			);

		std::string result =
			boost::str(boost::format(messageFormat)
			% info.handle
			% Details::StringCnv::w2a(Details::Handle2Path(info.handle))
			% (info.data ? "" : "not ")
			% (info.data ? ToString(info.data.get()) : "")
			);

		return result;
	}

} /// end namespace RegistryScanner
