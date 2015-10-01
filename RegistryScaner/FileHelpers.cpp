/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "FileHelpers.h"

#include <stdexcept>

#include <boost/format.hpp>

namespace RegistryScanner { namespace Details {

	void FileHelpers::CloseFile(boost::filesystem::ofstream& ofs)
	{
		if (ofs.is_open())
		{
			ofs.flush();
			ofs.close();

			assert(!ofs.is_open());
		}
	}

	boost::filesystem::ofstream& FileHelpers::LazyFile(boost::filesystem::ofstream& ofs, boost::filesystem::path const& filePath)
	{
		assert(!filePath.empty());
		if (!ofs.is_open())
		{
			ofs.open(filePath);

			assert(ofs.is_open() && "Error - open file is failed.");
			if (!ofs.is_open())
				throw std::runtime_error(boost::str(boost::format("Error - open file: [%1%] - is failed.") % filePath.string()));

			ofs.unsetf(std::ios::skipws);
		}

		return ofs;
	}

}} /// end namespace RegistryScanner::Details
