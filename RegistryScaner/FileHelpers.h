/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

namespace RegistryScanner { namespace Details {

	struct FileHelpers
	{
		static void CloseFile(boost::filesystem::ofstream& ofs);
		static boost::filesystem::ofstream& LazyFile(boost::filesystem::ofstream& ofs, boost::filesystem::path const& filePath);
	};

}} /// end namespace RegistryScanner::Details
