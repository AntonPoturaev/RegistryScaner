/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

namespace RegistryScanner { namespace Details {

	template<typename T>
	struct SimpleSingleton
	{
		static T& Instance()
		{
			static T instance;
			return instance;
		}
	};

}} /// end namespace RegistryScanner::Details
