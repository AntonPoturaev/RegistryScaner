/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "IScanner.h"

namespace RegistryScanner {

	struct ScannerFactory
	{
		template<typename TScannerCreationParams>
		static IScannerPtr_t CreateScanner(TScannerCreationParams&& params);
	};

} /// end namespace RegistryScanner
