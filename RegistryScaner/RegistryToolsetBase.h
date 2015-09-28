/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "IRegistryToolset.h"

namespace RegistryScanner {

	struct RegistryToolsetBase 
		: public IRegistryToolset
	{
		virtual ~RegistryToolsetBase();
	};

} /// end namespace RegistryScanner
