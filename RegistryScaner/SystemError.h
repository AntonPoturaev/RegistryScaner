/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include <stdexcept>

#include <windows.h>

namespace RegestryScanner {

	class SystemError 
		: public std::domain_error
	{
	public:
		SystemError();
		explicit SystemError(LONG error);

		LONG GetErrorCode() const;

	private:
		LONG m_ErrorCode;
	};

} /// end namespace RegestryScanner
