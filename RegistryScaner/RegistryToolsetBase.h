/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "IRegistryToolset.h"

namespace RegestryScanner {

	struct RegistryToolsetBase 
		: public IRegistryToolset
	{
		virtual ~RegistryToolsetBase();
	};

} /// end namespace RegestryScanner
