/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include <string>
#include <vector>

#include <windows.h>

#include <boost/variant.hpp>
#include <boost/shared_array.hpp>

namespace RegistryScanner {

	enum class ValueType
	{
		kVTAny = -1,
		kVTNone = REG_NONE,
		kVTExpandString = REG_EXPAND_SZ,
		kVTString = REG_SZ,
		kVTBinary = REG_BINARY,
		kVTDwordLittleEndian = REG_DWORD_LITTLE_ENDIAN,
		kVTDwordBigEndian = REG_DWORD_BIG_ENDIAN,
		kVTLink = REG_LINK,
		kVTMultiString = REG_MULTI_SZ,
		kVTResourceList = REG_RESOURCE_LIST,
		kVTFullResourceDescriptor = REG_FULL_RESOURCE_DESCRIPTOR,
		kVTResourceRequierementsList = REG_RESOURCE_REQUIREMENTS_LIST,
		kVTQwordLittleEndian = REG_QWORD_LITTLE_ENDIAN,
	};

	typedef std::wstring String_t;
	typedef std::vector<String_t> MultiString_t;
	typedef DWORD Dword_t;

	typedef struct {} None_t;

	typedef BYTE Byte_t;
	typedef std::vector<Byte_t> RawByteData_t;

	struct IrregularValue 
		: public std::logic_error
	{
	public:
		explicit IrregularValue(RawByteData_t const& data);

	public:
		RawByteData_t m_Data;
	};
	
	typedef union
	{
		unsigned long long qword;
		Byte_t bytes[8];
	}
	Qword_t;

	class ExpandString
	{
	public:
		ExpandString();
		ExpandString(ExpandString &&other);
		ExpandString(String_t &&other);

		ExpandString& operator=(ExpandString const& other);
		ExpandString& operator=(ExpandString&& other);
		ExpandString& operator=(String_t const& other);

		bool Equal(ExpandString const& other) const;
		bool Equal(String_t const& other) const;

		String_t& GetValue();
		String_t const& GetValue() const;

	private:
		String_t m_String;
	};

	bool operator==(ExpandString const& l, ExpandString const& r);
	bool operator!=(ExpandString const& l, ExpandString const& r);
	bool operator==(ExpandString const& l, String_t const& r);
	bool operator!=(ExpandString const& l, String_t const& r);

	typedef boost::variant<RawByteData_t, String_t, MultiString_t, ExpandString, Dword_t, Qword_t> Value_t;

	typedef union
	{
		DWORD dword;
		BYTE bytes[4];
	} DwordBE_t;
	
	struct Link {};
	struct ResourceList {};
	struct FullResourceDescriptor {};
	struct ResourceRequierementsList {};

	typedef std::wstring Name_t;

	bool IsConvertible(ValueType left, ValueType right);

	template<typename T>
	struct ValueConstructor
	{
		static T Construct(ValueType type, Byte_t const* data, size_t size);
	};

	template<typename T>
	struct ValueToType
	{
		static ValueType GetType(T const&) { 
			return ValueType:kVTNone; 
		}

		static ValueType GetType() {
			return ValueType:kVTNone;
		}

		static std::wstring GetTypeName(T const&) {
			return L"REG_NONE";
		}

		static std::wstring GetTypeName() {
			return L"REG_NONE";
		}
	};

	template<>
	struct ValueToType<Value_t>
	{
		static ValueType GetType(Value_t const& value);
		static ValueType GetType();

		static std::wstring GetTypeName(Value_t const&);
		static std::wstring GetTypeName();
	};

	std::wstring Value2WideString(Value_t const& value);
	
} /// end namespace RegistryScanner 
