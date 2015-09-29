/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "HiLevelScannerController.h"
#include "ScannerFactory.h"
#include "HiLevelScanner.h"
#include "DBG_SetThreadName.h"

namespace RegistryScanner {

	HiLevelScannerController::HiLevelScannerController(Descriptors_t&& params, std::ostream& os, size_t chunkSize)
		: m_HkeyStore(std::forward<Descriptors_t>(params))
		, m_ChunkSize(chunkSize)
		, m_Out(os)
	{
	}

	HiLevelScannerController::~HiLevelScannerController() {
		_StopScan(true, false);
	}

	HiLevelScannerController::Connection_t HiLevelScannerController::AttachOnScanStartSignal(OnScanStartSignal_t::slot_type slot) {
		return m_OnScanStartSignal.connect(slot);
	}

	HiLevelScannerController::Connection_t HiLevelScannerController::AttachOnScanEndSignal(OnScanEndSignal_t::slot_type slot) {
		return m_OnScanEndSignal.connect(slot);
	}

	HiLevelScannerController::Connection_t HiLevelScannerController::AttachNextScanResultsCompleteSignal(OnNextScanResultsCompleteSignal_t::slot_type slot) {
		return m_OnNextScanResultsCompleteSignal.connect(slot);
	}

	HiLevelScannerController::Connection_t HiLevelScannerController::AttachOnPathFoundSignal(OnPathFoundSignal_t::slot_type slot) {
		return m_Scanner->AttachOnPathFoundSignal(slot);
	}

	HiLevelScannerController::Connection_t HiLevelScannerController::AttachOnErrorFoundSignal(OnErrorFoundSignal_t::slot_type slot) {
		return m_Scanner->AttachOnPathFoundSignal(slot);
	}

	HiLevelScannerController::Connection_t HiLevelScannerController::AttachOnOperationSuccessSignal(OnOperationSuccess_t::slot_type slot) {
		return m_Scanner->AttachOnErrorFoundSignal(slot);
	}

	HiLevelScannerController::Connection_t HiLevelScannerController::AttachOnInformationSignal(OnInformation_t::slot_type slot) {
		return m_Scanner->AttachOnInformationSignal(slot);
	}


	void HiLevelScannerController::ScanRegistryAssync() {
		m_Worker.reset(new std::thread(&HiLevelScannerController::_Routine, this));
	}

	void HiLevelScannerController::StopScan() {
		_StopScan(true, true);
	}

	void HiLevelScannerController::_Routine()
	{
		SetThisThreadName("HiLevelScannerController thread");

		m_OnScanStartSignal();

		for (auto currentHkey : m_HkeyStore)
		{
			static DWORD const accessMask =
				KEY_READ
#if defined(_WIN64)
				| KEY_WOW64_64KEY
#else
				| KEY_WOW64_32KEY
#endif
				;

			m_Scanner = ScannerFactory::CreateScanner(HiLevelScanner::CreationParams(currentHkey, accessMask));

			m_Scanner->Scan();

			m_Scanner.reset();
		}

		{
			std::lock_guard<std::mutex> const lock(m_Access);

			if (m_ScanInfoStore && !m_ScanInfoStore->empty())
			{
				auto const size = m_ScanInfoStore->size();
				assert(size <= m_ChunkSize && "Bad data. Logic error.");

				m_OnNextScanResultsCompleteSignal(m_ScanInfoStore);
				m_ScanInfoStore.reset();
			}
		}

		m_OnScanEndSignal(false);
	}

	void HiLevelScannerController::_StopScan(bool aborted, bool needSignal)
	{
		if (m_Worker)
		{
			if (m_Worker->joinable())
				m_Worker->join();

			if (m_Worker->joinable())
				m_Worker->detach();

			m_Worker.reset();

			if (needSignal)
				m_OnScanEndSignal(aborted);
		}
	}

	void HiLevelScannerController::_OnPathFound(ScanInfoPtr_t scanInfo)
	{
		std::lock_guard<std::mutex> const lock(m_Access);

		m_Out << "\n*** ScanInfo is received successfully.\n" << std::endl;

		assert(scanInfo && scanInfo->handle && scanInfo->data.is_initialized() && "Bad params.");

		if (!m_ScanInfoStore)
		{
			m_ScanInfoStore.reset(new ScanInfoStore_t);
			m_ScanInfoStore->reserve(m_ChunkSize);
		}
		
		m_ScanInfoStore->push_back(scanInfo);

		assert(m_ScanInfoStore && m_ScanInfoStore->back() && m_ScanInfoStore->back()->handle && m_ScanInfoStore->back()->data.is_initialized() && "Bad data.");
		assert(m_ScanInfoStore && m_ScanInfoStore->front() && m_ScanInfoStore->front()->handle && m_ScanInfoStore->front()->data.is_initialized() && "Bad data.");

		if (m_ScanInfoStore->size() == m_ChunkSize)
		{
			m_OnNextScanResultsCompleteSignal(m_ScanInfoStore);
			m_ScanInfoStore.reset();
		}
	}

} /// end namespace RegistryScanner
