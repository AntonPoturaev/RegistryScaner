/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "SystemError.h"

#include <sstream>
#include <string>
#include <memory>

namespace RegistryScanner {

	namespace {
		std::string _ConstructError(DWORD errorCode)
		{
			std::shared_ptr<char> message(nullptr, ::LocalFree);

			DWORD const messageRequestResult = ::FormatMessageA(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS
				, nullptr
				, errorCode
				, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
				, reinterpret_cast<char*>(&message)
				, 0
				, nullptr
			);

			if (messageRequestResult == 0)
				throw std::logic_error("Invalid error message formatting.");

			std::ostringstream stream;
			stream << "Error " << errorCode << ": " << message.get();

			return stream.str();
		}
	} /// end unnamed namespace

	SystemError::SystemError()
		: std::domain_error(_ConstructError(GetLastError()))
		, m_ErrorCode(GetLastError())
	{
	}

	SystemError::SystemError(LONG error) 
		: std::domain_error(_ConstructError(error))
		, m_ErrorCode(error)
	{
	}

	LONG SystemError::GetErrorCode() const {
		return m_ErrorCode;
	}

} /// end namespace RegistryScanner
