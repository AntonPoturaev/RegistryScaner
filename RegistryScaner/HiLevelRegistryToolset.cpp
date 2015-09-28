/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "HiLevelRegistryToolset.h"
#include "Value.h"

#include <cassert>

namespace RegistryScanner {
	
	LONG HiLevelRegistryToolset::OpenKey(HKEY handle, std::wstring const& subkeyName, DWORD accessMask, PHKEY outHandle) const
	{
		assert(handle != nullptr && "Bad params.");
		return ::RegOpenKeyEx(handle, subkeyName.empty() ? nullptr : subkeyName.c_str(), 0, accessMask, outHandle);
	}

	LONG HiLevelRegistryToolset::CloseKey(HKEY handle) const
	{
		LONG result = ERROR_SUCCESS;
		if (handle != nullptr)
		{
			if ((result = RegCloseKey(handle)) == ERROR_SUCCESS)
				handle = nullptr;
		}

		return result;
	}

	LONG HiLevelRegistryToolset::QueryInfoKey(HKEY handle, KeyInfo& keyInfo) const
	{
		assert(handle != nullptr && "Bad params.");
		return ::RegQueryInfoKey(handle
			, keyInfo.classNameBuffer.data, &keyInfo.classNameBuffer.size
			, nullptr /// reserved
			, &keyInfo.subKeysCount, &keyInfo.subKeyLongset, &keyInfo.classNameLongset
			, &keyInfo.valuesForKeyCount, &keyInfo.valueNameLongset, &keyInfo.valueDataLongset
			, &keyInfo.securityDescriptorSize, &keyInfo.lastWriteTime);
	}

	LONG HiLevelRegistryToolset::GetKeyNameAtIndex(HKEY handle, DWORD index, std::wstring& name) const
	{
		assert(handle != nullptr && "Bad params.");

		Details::WideStaticBuffer<KeyInfo::maxKeyLength> keyNameBuffer(KeyInfo::maxKeyLength);

		LONG const result = ::RegEnumKeyEx(handle, index, keyNameBuffer.data, &keyNameBuffer.size
			, nullptr /// reserved
			, nullptr, nullptr, nullptr);

		if (result == ERROR_SUCCESS)
			name = keyNameBuffer.ToString();

		return result;
	}

	LONG HiLevelRegistryToolset::GetValueInfoAtIndex(HKEY handle, DWORD index, ValueInfo& valueInfo) const
	{
		assert(handle != nullptr && "Bad params.");

		Details::WideStaticBuffer<ValueInfo::maxValueName> valueNameBuffer(ValueInfo::maxValueName);
		DWORD dataSize = 0;
		DWORD type = -1;

		LONG result = ::RegEnumValue(handle, index, valueNameBuffer.data, &valueNameBuffer.size
			, nullptr/// reserved 
			, &type, nullptr, &dataSize);

		if (result == ERROR_SUCCESS)
		{
			valueInfo.name = valueNameBuffer.ToString();

			std::unique_ptr<Byte_t> buffer(dataSize ? new Byte_t[dataSize] : nullptr);

			result = ::RegQueryValueEx(handle, valueInfo.name.c_str(), nullptr
				, nullptr/// reserved 
				, buffer.get(), &dataSize);

			if (result == ERROR_SUCCESS)
				valueInfo.value = ValueConstructor<Value_t>::Construct(static_cast<ValueType>(type), buffer.get(), dataSize);
		}

		return result;
	}

} /// end namespace RegistryScanner
