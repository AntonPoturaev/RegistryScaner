/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "TimeStamp.h"

#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace RegistryScanner { namespace Details {

	std::string TimeStamp() {
		return boost::algorithm::replace_all_copy(boost::lexical_cast<std::string>(boost::posix_time::second_clock::universal_time()), ":", "-");
	}

}} /// end namespace RegistryScanner::Details
