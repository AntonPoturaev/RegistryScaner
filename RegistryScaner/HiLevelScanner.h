/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "IScanner.h"
#include "HiLevelRegistryToolset.h"
#include "ScannerFactory.h"

#include <boost/filesystem/path.hpp>

namespace RegestryScanner {

	class HiLevelScanner 
		: public IScanner
	{
		typedef boost::filesystem::path Path_t;

		friend struct ScannerFactory;

	public:
		struct CreationParams
		{
			CreationParams(HKEY rootHandle_, DWORD accessMask_);

			HKEY rootHandle;
			DWORD accessMask;
		};

	public:
		virtual Connection_t AttachOnPathFoundSignal(OnPathFoundSignal_t::slot_type slot) override;
		virtual Connection_t AttachOnErrorFoundSignal(OnErrorFoundSignal_t::slot_type slot) override;
		virtual Connection_t AttachOnOperationSuccessSignal(OnOperationSuccess_t::slot_type slot) override;
		virtual Connection_t AttachOnInformationSignal(OnInformation_t::slot_type slot) override;

		virtual void Scan() const override;

	private:
		HiLevelScanner(CreationParams&& params);

		void _ScanRoutine(HKEY parent, std::wstring const& name) const;

	private:
		CreationParams const m_Params;
		HiLevelRegistryToolset const m_Toolset;
		
		OnPathFoundSignal_t m_OnPathFoundSignal;
		OnErrorFoundSignal_t m_OnErrorFoundSignal;
		OnOperationSuccess_t m_OnOperationSuccessSignal;
		OnInformation_t m_OnInformationSignal;
	};
	
} /// end namespace RegestryScanner
