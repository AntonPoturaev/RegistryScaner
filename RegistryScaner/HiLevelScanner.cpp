/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "HiLevelScanner.h"
#include "SystemError.h"

#include <locale>
#include <codecvt>

#include <boost/scope_exit.hpp>
#include <boost/format.hpp>

namespace RegistryScanner {

	HiLevelScanner::CreationParams::CreationParams(HKEY rootHandle_, DWORD accessMask_)
		: rootHandle(rootHandle_)
		, accessMask(accessMask_)
	{
	}

	IScanner::Connection_t HiLevelScanner::AttachOnPathFoundSignal(OnPathFoundSignal_t::slot_type slot) {
		return m_OnPathFoundSignal.connect(slot);
	}

	IScanner::Connection_t HiLevelScanner::AttachOnErrorFoundSignal(OnErrorFoundSignal_t::slot_type slot) {
		return m_OnErrorFoundSignal.connect(slot);
	}

	IScanner::Connection_t HiLevelScanner::AttachOnOperationSuccessSignal(OnOperationSuccess_t::slot_type slot) {
		return m_OnOperationSuccessSignal.connect(slot);
	}

	IScanner::Connection_t HiLevelScanner::AttachOnInformationSignal(OnInformation_t::slot_type slot) {
		return m_OnInformationSignal.connect(slot);
	}

	void HiLevelScanner::Scan() const 
	{
		try {
			_ScanRoutine(m_Params.rootHandle, L"");
		}
		catch (...) {
			m_OnErrorFoundSignal(-1, L"Undefined exception cached. Scanning process is aborted.");
		}
	}

	HiLevelScanner::HiLevelScanner(CreationParams&& params)
		: m_Params(std::forward<CreationParams>(params))
	{
	}

	namespace {
		class _HandleValue
		{
		public:
			_HandleValue(HKEY handle)
				: m_ValueHolder(reinterpret_cast<DWORD>(reinterpret_cast<DWORD*>(handle)))
			{
			}

			DWORD const* Get() const {
				return &m_ValueHolder;
			}

		private:
			DWORD m_ValueHolder;
		};

		std::wstring _a2w(std::string const& str) 
		{
			try {
				return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(str);
			} catch (...) {
				return L"*** Failed to call _a2w";
			}
		}

	} /// end unnamed namespace

	void HiLevelScanner::_ScanRoutine(HKEY parent, std::wstring const& name) const
	{
		HKEY handle(nullptr);

		LONG erroCode(ERROR_SUCCESS);

		BOOST_SCOPE_EXIT((erroCode)(handle)(this_))
		{
			_HandleValue const handelValue(handle);

			erroCode = this_->m_Toolset.CloseKey(handle);

			if (erroCode == ERROR_SUCCESS)
				this_->m_OnOperationSuccessSignal(boost::str(boost::wformat(L"Handle: %1% is successfully closed.") % handelValue.Get()));
			else
				this_->m_OnErrorFoundSignal(erroCode, boost::str(boost::wformat(L"Failed to close handle: %1%.") % handelValue.Get()));
		} BOOST_SCOPE_EXIT_END;

		erroCode = m_Toolset.OpenKey(parent, name, m_Params.accessMask, &handle);

		if (erroCode == ERROR_SUCCESS)
			m_OnOperationSuccessSignal(boost::str(boost::wformat(L"Path: %1%, by handle: %2% is successfully opened.") % name % parent));
		else
		{
			m_OnErrorFoundSignal(erroCode, boost::str(boost::wformat(L"Failed to open path: %1%, by handle: %2%. This iteration aborted.") % name % parent));
			return;
		}
		
		KeyInfo keyInfo;
		erroCode = m_Toolset.QueryInfoKey(handle, keyInfo);

		DWORD valuesForKeyCount = 0, subKeysCount = 0;
		subKeysCount = keyInfo.subKeysCount;
		valuesForKeyCount = keyInfo.valuesForKeyCount;

		if (erroCode == ERROR_SUCCESS)
			m_OnOperationSuccessSignal(
				boost::str(boost::wformat(L"Information for path: %1% by handle: %2% is successfully received. Key count: %3%, value count: %4%")
					% name % handle 
					% (subKeysCount = keyInfo.subKeysCount)
					% (valuesForKeyCount = keyInfo.valuesForKeyCount)
				)
			);
		else
		{
			m_OnErrorFoundSignal(erroCode, boost::str(boost::wformat(L"Failed to query info for handle: %1%. This iteration aborted.") % handle));
			return;
		}
				
		if (valuesForKeyCount == 0 && subKeysCount == 0)
		{
			m_OnPathFoundSignal(std::make_shared<ScanInfo>(handle, keyInfo));
			return;
		}
		else
		{
			for (size_t i = 0; i < valuesForKeyCount; ++i)
			{
				try
				{
					ValueInfo valueInfo;
					erroCode = m_Toolset.GetValueInfoAtIndex(handle, i, valueInfo);

					if (erroCode == ERROR_SUCCESS)
						m_OnOperationSuccessSignal(boost::str(boost::wformat(L"Value info: %1% for handle: %2% is successfully received.") % i % handle));
					else
					{
						m_OnErrorFoundSignal(erroCode, boost::str(boost::wformat(L"Failed to get value info: %1% for handle: %2%. This iteration aborted.") % i % handle));
						continue;
					}

					m_OnPathFoundSignal(std::make_shared<ScanInfo>(handle, valueInfo));
				} 
				catch (SystemError const& err) 
				{
					m_OnErrorFoundSignal(err.GetErrorCode()
						, boost::str(boost::wformat(L"Failed to get value info: %1% for handle: %2%. Reason: %3% This iteration aborted.") 
						% i % handle % _a2w(err.what()))
					);
				} 
				catch (IrregularValue const& err)
				{
					m_OnErrorFoundSignal(-1, boost::str(boost::wformat(L"Failed to get value info: %1% for handle: %2%. Reason: %3% This iteration aborted.") 
						% i % handle % _a2w(err.what()))
					);
				} 
				catch (std::exception const& err) 
				{
					m_OnErrorFoundSignal(-1, boost::str(boost::wformat(L"Failed to get value info: %1% for handle: %2%. Reason: %3% This iteration aborted.")
						% i % handle % _a2w(err.what()))
					);
				}
			}

			for (size_t i = 0; i < subKeysCount; ++i)
			{
				std::wstring nextName;
				erroCode = m_Toolset.GetKeyNameAtIndex(handle, i, nextName);

				if (erroCode == ERROR_SUCCESS)
					m_OnOperationSuccessSignal(boost::str(boost::wformat(L"Key name: %1% for handle: %2% is successfully received. Name: %3%") % i % handle % nextName));
				else
				{
					m_OnErrorFoundSignal(erroCode, boost::str(boost::wformat(L"Failed to get key name: %1% for handle: %2%. This iteration aborted.") % i % handle));
					continue;
				}

				_ScanRoutine(handle, nextName);
			}
		}
	}
	
	template<>
	IScannerPtr_t ScannerFactory::CreateScanner(HiLevelScanner::CreationParams&& params) {
		return IScannerPtr_t(new HiLevelScanner(std::forward<HiLevelScanner::CreationParams>(params)));
	}

} /// end namespace RegistryScanner
