/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "StaticBuffer.h"

namespace RegistryScanner { namespace Details {

	template<DWORD Capacity>
	class WideStaticBuffer
		: public StaticBuffer<wchar_t, Capacity>
	{
		typedef StaticBuffer<wchar_t, Capacity> Super_t;
	public:
		WideStaticBuffer(size_t s = 0)
			: Super_t(s)
		{
		}
	};

}} /// end namespace RegistryScanner::Details
