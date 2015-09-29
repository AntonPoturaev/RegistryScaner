/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "IScanner.h"

#include <thread>
#include <mutex>
#include <ostream>

namespace RegistryScanner {

	class HiLevelScannerController
		: public IScanerDispatcher
	{
	public:
		typedef boost::signals2::scoped_connection ScopedConnection_t;
		typedef std::vector<ScopedConnection_t> ConnectionStore_t;

		typedef std::vector<HKEY> Descriptors_t;

		typedef std::shared_ptr<ScanInfoStore_t> ScanInfoStorePtr_t;

		typedef boost::signals2::signal<void()> OnScanStartSignal_t;
		typedef boost::signals2::signal<void(bool aborted)> OnScanEndSignal_t;
		typedef boost::signals2::signal<void(ScanInfoStorePtr_t store)> OnNextScanResultsCompleteSignal_t;
		typedef boost::signals2::connection Connection_t;
	
	public:
		HiLevelScannerController(Descriptors_t&& params, std::ostream& os, size_t chunkSize = 15);
		~HiLevelScannerController();

		Connection_t AttachOnScanStartSignal(OnScanStartSignal_t::slot_type slot);
		Connection_t AttachOnScanEndSignal(OnScanEndSignal_t::slot_type slot);
		Connection_t AttachNextScanResultsCompleteSignal(OnNextScanResultsCompleteSignal_t::slot_type slot);

		virtual Connection_t AttachOnPathFoundSignal(OnPathFoundSignal_t::slot_type slot) override;
		virtual Connection_t AttachOnErrorFoundSignal(OnErrorFoundSignal_t::slot_type slot) override;
		virtual Connection_t AttachOnOperationSuccessSignal(OnOperationSuccess_t::slot_type slot) override;
		virtual Connection_t AttachOnInformationSignal(OnInformation_t::slot_type slot) override;

		void ScanRegistryAssync();
		void StopScan();
		
	private:
		void _Routine();
		void _StopScan(bool aborted, bool needSignal);
		void _OnPathFound(ScanInfoPtr_t scanInfo);

	private:
		Descriptors_t m_HkeyStore;
		std::ostream& m_Out;
		size_t const m_ChunkSize;
		ScanInfoStorePtr_t m_ScanInfoStore;

		OnScanStartSignal_t m_OnScanStartSignal;
		OnScanEndSignal_t m_OnScanEndSignal;
		OnNextScanResultsCompleteSignal_t m_OnNextScanResultsCompleteSignal;

		IScannerPtr_t m_Scanner;

		std::mutex m_Access;
		std::unique_ptr<std::thread> m_Worker;
	};

} /// end namespace RegistryScanner

