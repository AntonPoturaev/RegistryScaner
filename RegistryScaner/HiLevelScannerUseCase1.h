/**
	Project - Regestry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#pragma once

#include "HiLevelScannerController.h"

#include <windows.h>

#include <mutex>
#include <condition_variable>
#include <functional>
#include <deque>

namespace RegestryScanner { namespace UseCase {
	
	class HiLevelScannerUseCase1
	{
		typedef std::function<void()> StopLasyReporter_t;

	public:
		static int Run();

	private:
		HiLevelScannerUseCase1();

		int _Run();

		void _OnScanStart();
		void _OnScanEnd(bool aborted);
		void _OnNextScanResultsComplete(HiLevelScannerController::ScanInfoStorePtr_t store);

	private:
		HiLevelScannerController m_Controller;
		HiLevelScannerController::ConnectionStore_t m_ConnectionStore;

		volatile bool m_Stoped;

		std::deque<HiLevelScannerController::ScanInfoStorePtr_t> m_Queue;

		StopLasyReporter_t m_StopLasyReporter;

		std::mutex m_Access;
		std::condition_variable m_Condition;
	};
		
}} /// end namespace RegestryScanner::UseCase
