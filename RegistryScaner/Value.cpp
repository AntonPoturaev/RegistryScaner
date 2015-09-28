/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "Value.h"
#include "SystemError.h"

#include <cassert>

namespace RegistryScanner {

	IrregularValue::IrregularValue(RawByteData_t const& data) 
		: std::logic_error("The value content isn't comply with the value type.")
		, m_Data(data)
	{
	}


	ExpandString::ExpandString() {}

	ExpandString::ExpandString(ExpandString&& other)
		: m_String(std::forward<String_t>(other.m_String))
	{
	}

	ExpandString::ExpandString(String_t&& other)
		: m_String(std::forward<String_t>(other))
	{
	}

	ExpandString& ExpandString::operator=(ExpandString const& other)
	{
		if (this != &other)
			m_String = other.m_String;

		return *this;
	}

	ExpandString& ExpandString::operator=(ExpandString&& other)
	{
		if (this != &other)
			m_String = std::forward<String_t>(other.m_String);

		return *this;
	}

	ExpandString& ExpandString::operator=(String_t const& other)
	{
		m_String = other;

		return *this;
	}

	bool ExpandString::Equal(ExpandString const& other) const {
		return Equal(other.m_String);
	}

	bool ExpandString::Equal(String_t const& other) const {
		return m_String == other;
	}

	String_t& ExpandString::GetValue() {
		return m_String;
	}

	String_t const& ExpandString::GetValue() const {
		return m_String;
	}

	bool operator==(ExpandString const& l, ExpandString const& r) {
		return l.Equal(r);
	}

	bool operator!=(ExpandString const& l, ExpandString const& r) {
		return !(l == r);
	}

	bool operator==(ExpandString const& l, String_t const& r) {
		return l.Equal(r);
	}

	bool operator!=(ExpandString const& l, String_t const& r) {
		return !(l == r);
	}


	bool IsConvertible(ValueType left, ValueType right)
	{
		return	(left == right)
				|| (right == ValueType::kVTAny)
				|| (right == ValueType::kVTBinary)
				|| (left == ValueType::kVTDwordBigEndian && right == ValueType::kVTDwordLittleEndian)
				|| (left == ValueType::kVTExpandString && right == ValueType::kVTString)
				|| (left == ValueType::kVTLink && right == ValueType::kVTString)
		;
	}


	template<>
	Value_t ValueConstructor<Value_t>::Construct(ValueType type, Byte_t const* data, size_t size)
	{
		switch (type)
		{
		case ValueType::kVTNone: return ValueConstructor<RawByteData_t>::Construct(type, data, size);
		case ValueType::kVTString: case ValueType::kVTExpandString: return ValueConstructor<String_t>::Construct(type, data, size);
		case ValueType::kVTMultiString: return ValueConstructor<MultiString_t>::Construct(type, data, size);
		case ValueType::kVTBinary: return ValueConstructor<RawByteData_t>::Construct(type, data, size);
		case ValueType::kVTDwordLittleEndian: case ValueType::kVTDwordBigEndian: return ValueConstructor<Dword_t>::Construct(type, data, size);
		case ValueType::kVTLink: return ValueConstructor<String_t>::Construct(type, data, size);
		case ValueType::kVTResourceList: return ValueConstructor<RawByteData_t>::Construct(type, data, size);
		case ValueType::kVTFullResourceDescriptor: return ValueConstructor<RawByteData_t>::Construct(type, data, size);
		case ValueType::kVTResourceRequierementsList: return ValueConstructor<RawByteData_t>::Construct(type, data, size);
		case ValueType::kVTQwordLittleEndian: return ValueConstructor<Qword_t>::Construct(type, data, size);
		default: throw IrregularValue(RawByteData_t(data, data + size));
		}
	}

	namespace {
	
		template<typename Container, typename T>
		Container _SafeConstruct(T const* data, size_t size) {
			return (data && size) ? Container(data, data + size) : Container();
		}

	} /// end unnamed namespace

	template<>
	RawByteData_t ValueConstructor<RawByteData_t>::Construct(ValueType type, Byte_t const* data, size_t size) {
		return _SafeConstruct<RawByteData_t>(data, size);
	}

	template<>
	String_t ValueConstructor<String_t>::Construct(ValueType type, Byte_t const* data, size_t size)
	{
		assert(IsConvertible(type, ValueType::kVTString));

		if (size % sizeof(wchar_t) == 0)
		{
			wchar_t const* const dataAlias = reinterpret_cast<wchar_t const*>(data);

			switch(type)
			{
			case ValueType::kVTString: case ValueType::kVTLink: return _SafeConstruct<String_t>(dataAlias, size / sizeof(wchar_t) - 1);

			case ValueType::kVTExpandString:
				{
					/// http://msdn.microsoft.com/en-us/library/ms724265(v=VS.85).aspx
					if (size > 32768)
						throw std::logic_error("The size of the buffer is limited to 32K.");

					String_t result;
					size_t sourceCharCount = size / sizeof(wchar_t);
					size_t needCharCount = sourceCharCount;

					std::unique_ptr<wchar_t> expandedString;

					do 
					{
						expandedString.reset(new wchar_t[needCharCount]);

						sourceCharCount = needCharCount;
						needCharCount = ::ExpandEnvironmentStrings(dataAlias, expandedString.get(), needCharCount);

						if (needCharCount == 0)
							throw SystemError();
					}
					while (needCharCount > sourceCharCount);

					return _SafeConstruct<String_t>(expandedString.get(), needCharCount - 1);
				}

			default:
				break;
			}
		}

		throw IrregularValue(_SafeConstruct<RawByteData_t>(data, size));
	}

	template<>
	MultiString_t ValueConstructor<MultiString_t>::Construct(ValueType type, Byte_t const* data, size_t size)
	{
		assert(IsConvertible(type, ValueType::kVTMultiString));

		if (size % sizeof(wchar_t) != 0)
			throw IrregularValue(_SafeConstruct<RawByteData_t>(data, size));

		wchar_t const* const dataAlias = reinterpret_cast<wchar_t const*>(data);
		wchar_t const* const endData = dataAlias + (size / sizeof(wchar_t)) - 1;

		//assert(*endData == L'\0' && "Incorrect REG_MULTI_SZ format.");

		MultiString_t multiString;

		if (dataAlias && endData)
		{
			wchar_t const* stringStart = dataAlias;
			wchar_t const* stringEnd = stringStart;

			while (stringEnd != endData)
			{
				if (*stringEnd == L'\0')
				{
					multiString.emplace_back(stringStart, stringEnd);
					stringStart = ++stringEnd;
				}
				else
					++stringEnd;
			}

			if (stringStart != stringEnd)
			{
				//assert(!"Incorrect REG_MULTI_SZ format.");
				multiString.emplace_back(stringStart, stringEnd);
			}
		}

		return multiString;
	}
	
	template<>
	ExpandString ValueConstructor<ExpandString>::Construct(ValueType type, Byte_t const* data, size_t size)
	{
		assert(IsConvertible(type, ValueType::kVTExpandString));

		/// http://msdn.microsoft.com/en-us/library/ms724265(v=VS.85).aspx
		if (size > 32768)
			throw std::logic_error("The size of the buffer is limited to 32K.");

		if (size % sizeof(wchar_t) != 0)
			throw IrregularValue(_SafeConstruct<RawByteData_t>(data, size));

		wchar_t const* const dataAlias = reinterpret_cast<wchar_t const*>(data);

		return _SafeConstruct<String_t>(dataAlias, size / sizeof(wchar_t) - 1);
	}

	template<>
	Dword_t ValueConstructor<Dword_t>::Construct(ValueType type, Byte_t const* data, size_t size)
	{
		assert(IsConvertible(type, ValueType::kVTDwordLittleEndian));

		if (size != 4)
			throw IrregularValue(_SafeConstruct<RawByteData_t>(data, size));

		Dword_t value = 0;

		if (data)
		{
			switch (type)
			{
			case ValueType::kVTDwordLittleEndian:
				memcpy(&value, data, 4);
				break;

			case ValueType::kVTDwordBigEndian:
				value = static_cast<Dword_t>((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
				break;

			default:
				break;
			}
		}

		return value;
	}

	template<>
	Qword_t ValueConstructor<Qword_t>::Construct(ValueType type, Byte_t const* data, size_t size)
	{
		if (size != 8)
			throw IrregularValue(_SafeConstruct<RawByteData_t>(data, size));

		Qword_t value;

		if (data)
			memcpy(value.bytes, data, 8);

		return value;
	}

	namespace {

		struct _ValueToType 
			: public boost::static_visitor<ValueType>
		{
			template <typename T>
			ValueType operator()(T const& value) const {
				return ValueToType<T>::GetType(value);
			}

			ValueType operator()(Value_t const&) const {
				return ValueType::kVTAny;
			}
		};

		struct _ValueToTypeName : public boost::static_visitor<std::wstring>
		{
			template <typename T>
			std::wstring operator()(T const& value) const {
				return ValueToType<T>::GetTypeName();
			}
		};

	} /// end unnamed namespace

	ValueType ValueToType<Value_t>::GetType(Value_t const& value) {
		return boost::apply_visitor(_ValueToType(), value);
	}

	ValueType ValueToType<Value_t>::GetType() {
		return ValueType::kVTAny;
	}

	std::wstring ValueToType<Value_t>::GetTypeName(Value_t const& value) {
		return boost::apply_visitor(_ValueToTypeName(), value);
	}

	std::wstring ValueToType<Value_t>::GetTypeName() {
		return L"ANY";
	}

	template<> 
	struct ValueToType<None_t>
	{
		static ValueType GetType() { 
			return ValueType::kVTNone;
		}

		static ValueType GetType(None_t const&) { 
			return GetType();
		}

		static std::wstring GetTypeName(None_t const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_NONE";
		}
	};

	template<> 
	struct ValueToType<RawByteData_t>
	{
		static ValueType GetType() {
			return ValueType::kVTBinary;
		}

		static ValueType GetType(RawByteData_t const&) { 
			return GetType(); 
		}

		static std::wstring GetTypeName(RawByteData_t const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_BINARY";
		}
	};

	template<>
	struct ValueToType<String_t>
	{
		static ValueType GetType() { 
			return ValueType::kVTString; 
		}

		static ValueType GetType(String_t const&) {
			return GetType(); 
		}

		static std::wstring GetTypeName(String_t const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_SZ";
		}
	};

	template<> 
	struct ValueToType<MultiString_t>
	{
		static ValueType GetType() {
			return ValueType::kVTMultiString; 
		}

		static ValueType GetType(MultiString_t const&) {
			return GetType(); 
		}

		static std::wstring GetTypeName(MultiString_t const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_MULTI_SZ";
		}
	};

	template<>
	struct ValueToType<ExpandString>
	{
		static ValueType GetType() { 
			return ValueType::kVTExpandString;
		}

		static ValueType GetType(ExpandString const&) {
			return GetType();
		}

		static std::wstring GetTypeName(ExpandString const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_EXPAND_SZ";
		}
	};

	template<>
	struct ValueToType<Dword_t>
	{
		static ValueType GetType() { 
			return ValueType::kVTDwordLittleEndian; 
		}

		static ValueType GetType(Dword_t const&) { 
			return GetType(); 
		}

		static std::wstring GetTypeName(Dword_t const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_DWORD_LITTLE_ENDIAN";
		}
	};

	template<> 
	struct ValueToType<DwordBE_t>
	{
		static ValueType GetType() {
			return ValueType::kVTDwordBigEndian;
		}

		static ValueType GetType(DwordBE_t const&) { 
			return GetType();
		}

		static std::wstring GetTypeName(DwordBE_t const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_DWORD_BIG_ENDIAN";
		}
	};

	template<> 
	struct ValueToType<Link>
	{
		static ValueType GetType() { 
			return ValueType::kVTLink;
		}

		static ValueType GetType(Link const&) { 
			return GetType();
		}

		static std::wstring GetTypeName(Link const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_LINK";
		}
	};

	template<> 
	struct ValueToType<ResourceList>
	{
		static ValueType GetType() { 
			return ValueType::kVTResourceList;
		}

		static ValueType GetType(ResourceList const&) {
			return GetType(); 
		}

		static std::wstring GetTypeName(ResourceList const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_RESOURCE_LIST";
		}
	};

	template<>
	struct ValueToType<FullResourceDescriptor>
	{
		static ValueType GetType() {
			return ValueType::kVTFullResourceDescriptor; 
		}

		static ValueType GetType(FullResourceDescriptor const&) { 
			return GetType(); 
		}

		static std::wstring GetTypeName(FullResourceDescriptor const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_FULL_RESOURCE_DESCRIPTOR";
		}
	};
	
	template<>
	struct ValueToType<ResourceRequierementsList>
	{
		static ValueType GetType() { 
			return ValueType::kVTResourceRequierementsList;
		}

		static ValueType GetType(ResourceRequierementsList const&) {
			return GetType(); 
		}

		static std::wstring GetTypeName(ResourceRequierementsList const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_RESOURCE_REQUIREMENTS_LIST";
		}
	};

	template<> 
	struct ValueToType<Qword_t>
	{
		static ValueType GetType() {
			return ValueType::kVTQwordLittleEndian;
		}

		static ValueType GetType(Qword_t const&) { 
			return GetType(); 
		}

		static std::wstring GetTypeName(Qword_t const& value) {
			return GetTypeName(value);
		}

		static std::wstring GetTypeName() {
			return L"REG_QWORD_LITTLE_ENDIAN";
		}
	};

	namespace {

		struct _Value2WideStringDecoder 
			: public boost::static_visitor<std::wstring>
		{
			result_type operator()(RawByteData_t const& data) const
			{
				result_type result(data.begin(), data.end());
				return L'[' + result + L']';
			}

			result_type operator()(String_t const& data) const {
				return data;
			}

			result_type operator()(MultiString_t const& data) const
			{
				result_type result;

				for (auto const& i : data)
					result += L"{" + i + L"}";

				return result;
			}

			result_type operator()(ExpandString const& data) const {
				return data.GetValue();
			}

			result_type operator()(Dword_t const& data) const {
				return std::to_wstring(data);
			}

			result_type operator()(Qword_t const& data) const {
				return std::to_wstring(data.qword);
			}
		};

	} /// end unnamed namespace 

	std::wstring Value2WideString(Value_t const& value) {
		return boost::apply_visitor(_Value2WideStringDecoder(), value);
	}

} /// end namespace RegistryScanner
