/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "ValueInfo.h"
#include "KeyInfo.h"

#include <boost/optional.hpp>

namespace RegestryScanner {

	struct ScanInfo
	{
	public:
		typedef boost::variant<KeyInfo, ValueInfo> Data_t;

	public:
		template<typename DataT>
		ScanInfo(HKEY handle_, DataT data_)
			: handle(handle_)
			, data(data_)
		{
		}
			
	public:
		HKEY handle;
		boost::optional<Data_t> data;
	};

	typedef std::shared_ptr<ScanInfo> ScanInfoPtr_t;
	typedef std::vector<ScanInfoPtr_t> ScanInfoStore_t;

	std::wstring ToWideString(ScanInfo::Data_t const& infoData);
	std::wstring ToWideString(ScanInfo const& info);

} /// end namespace RegestryScanner
