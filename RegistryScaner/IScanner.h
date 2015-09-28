/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "ScanInfo.h"

#include <memory>

#include <windows.h>

#include <boost/signals2.hpp>

namespace RegestryScanner {

	struct IScannerObserver
	{
	public:
		typedef boost::signals2::signal<void(ScanInfoPtr_t scanInfo)> OnPathFoundSignal_t;
		typedef boost::signals2::signal<void(LONG erroCode, std::wstring message)> OnErrorFoundSignal_t;
		typedef boost::signals2::signal<void(std::wstring message)> OnOperationSuccess_t;
		typedef boost::signals2::signal<void(std::wstring message)> OnInformation_t;
		typedef boost::signals2::connection Connection_t;

	public:
		virtual Connection_t AttachOnPathFoundSignal(OnPathFoundSignal_t::slot_type slot) = 0;
		virtual Connection_t AttachOnErrorFoundSignal(OnErrorFoundSignal_t::slot_type slot) = 0;
		virtual Connection_t AttachOnOperationSuccessSignal(OnOperationSuccess_t::slot_type slot) = 0;
		virtual Connection_t AttachOnInformationSignal(OnInformation_t::slot_type slot) = 0;
	};

	struct IScanner 
		: public IScannerObserver
	{
		virtual void Scan() const = 0;
	};

	typedef std::unique_ptr<IScanner> IScannerPtr_t;

} /// end namespace RegestryScanner
