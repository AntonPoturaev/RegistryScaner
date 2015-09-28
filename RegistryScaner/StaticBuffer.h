/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include <windows.h>
#include <string>

namespace RegistryScanner { namespace Details {

	template<typename CharT, size_t Capacity>
	struct StaticBuffer
	{
	public:
		typedef CharT Char_t;
		typedef std::basic_string<Char_t> String_t;

	public:
		StaticBuffer(size_t s = 0)
			: size(s)
		{
			::memset(data, 0, capacity * sizeof(Char_t));
		}

		String_t ToString() const {
			return String_t(data, size);
		}

	public:
		static size_t const capacity = Capacity;

		Char_t data[capacity];
		size_t size;
	};
	
}} /// end namespace RegistryScanner::Details
