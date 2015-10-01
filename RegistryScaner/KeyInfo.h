/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "WideStaticBuffer.h"

namespace RegistryScanner {

	struct KeyInfo
	{
		KeyInfo();

		Details::WideStaticBuffer<MAX_PATH> classNameBuffer;
		DWORD classNameLongset; /// longest class string

		DWORD subKeysCount; /// number of subkeys
		DWORD subKeyLongset; /// longest subkey size

		DWORD valuesForKeyCount; /// number of values for key

		DWORD valueNameLongset; /// longest value name
		DWORD valueDataLongset; /// longest value data

		DWORD securityDescriptorSize; /// size of security descriptor

		FILETIME lastWriteTime; /// last write time

		static DWORD const maxKeyLength = 255;
	};

	std::wstring ToWideString(KeyInfo const& info);
	std::string ToString(KeyInfo const& info);

} /// end namespace RegistryScanner
