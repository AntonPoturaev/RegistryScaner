/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "HiLevelScannerUseCase1.h"
#include "StringCnv.h"
#include "FileHelpers.h"
#include "TimeStamp.h"

#include <cassert>
#include <iostream>
#include <condition_variable>

#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

namespace RegistryScanner { namespace UseCase {

	namespace {
		boost::filesystem::path _GenUseCaseFileName() {
			return boost::str(boost::format("HiLevelScannerUseCase1_%1%_%2%.log") % ::GetCurrentProcessId() % Details::TimeStamp());
		}
	} /// end unnamed namespace
		
	int HiLevelScannerUseCase1::Run()
	{
		HiLevelScannerUseCase1 self;
		return self._Run();
	}

	HiLevelScannerUseCase1::HiLevelScannerUseCase1()
		: m_Controller(
			boost::assign::list_of
				(HKEY_CURRENT_USER)
				/*(HKEY_LOCAL_MACHINE) /// раскоментировать для тестов ;)
				(HKEY_USERS)
				(HKEY_CLASSES_ROOT)
				(HKEY_CURRENT_CONFIG)*/ /// этого хватит чтобы посмотреть на принцип работы... но все ветки лучше не перебирать - консоль загнётся! если перебирать всё то надо в файл или на форму выводить!
			, 100 /// это кол-во записей показываемых на экран за 1 раз
		)
		, m_Stoped(false)
		, m_Started(false)
		, m_FilePath(_GenUseCaseFileName())
	{
		m_ConnectionStore.emplace_back(m_Controller.AttachOnScanStartSignal(boost::bind(&HiLevelScannerUseCase1::_OnScanStart, this)));
		m_ConnectionStore.emplace_back(m_Controller.AttachOnScanEndSignal(boost::bind(&HiLevelScannerUseCase1::_OnScanEnd, this, _1)));
		m_ConnectionStore.emplace_back(m_Controller.AttachNextScanResultsCompleteSignal(boost::bind(&HiLevelScannerUseCase1::_OnNextScanResultsComplete, this, _1)));
	}

	int HiLevelScannerUseCase1::_Run()
	{
		m_Controller.ScanRegistryAssync();
		
		while (!m_Started)
		{
			std::unique_lock<std::mutex> lock(m_Access);
			m_Condition.wait(lock);
		}

		while (!m_Stoped)
		{
			std::cout << "\nScanning in process... patience" << std::endl;

			static std::chrono::seconds const g_TimeOut(3);

			std::unique_lock<std::mutex> lock(m_Access);
			m_Condition.wait_for(lock, g_TimeOut);
		}
		
		std::cout << "*** END OF USE CASE ***" << std::endl;

		::getchar();

		return 0;
	}

	void HiLevelScannerUseCase1::_OnScanStart()
	{
		std::cout << "HiLevelScannerUseCase1 is started." << std::endl;
		std::cout << "Wait for first results..." << std::endl;

		m_Started = true;

		m_Condition.notify_one();
	}

	void HiLevelScannerUseCase1::_OnScanEnd(bool aborted)
	{
		m_Stoped = true;

		m_ConnectionStore.clear();

		m_StopLasyReporter = [aborted]() { 
			std::cout << boost::str(boost::format("HiLevelScannerUseCase1 is %1%.") % (aborted ? "aborted" : "complete")) << std::endl;
		};

		Details::FileHelpers::CloseFile(m_File);

		m_Condition.notify_one();
	}

	void HiLevelScannerUseCase1::_OnNextScanResultsComplete(HiLevelScannerController::ScanInfoStorePtr_t store)
	{
		assert(store && !store->empty() && "Bad params.");

#if defined(_DEBUG)
		for (size_t i = 0; i < store->size(); ++i)
			assert(store->at(i) && store->at(i)->data.is_initialized());
#endif /// _DEBUG

		try
		{
			std::string infoStr;
			for (auto currentScanInfo : *store)
				infoStr += ToString(*currentScanInfo) + '\n';

			_File().rdbuf()->sputn(infoStr.data(), infoStr.size());
		}
		catch (...) {}
	}

	std::ofstream& HiLevelScannerUseCase1::_File() {
		return Details::FileHelpers::LazyFile(m_File, m_FilePath);
	}
				
}} /// end namespace RegistryScanner::UseCase
