/**
	Project - Registry Scanner
	Copyright (c) 2015 Poturaev A.S.
	e-mail: a.s.poturaev@gmail.com
*/

#include "stdafx.h"
#include "HiLevelScannerUseCase1.h"
#include "StringCnv.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

namespace RegistryScanner { 
	
	namespace {
		std::string _GenControllerFileName() {
			return boost::str(boost::format("HiLevelScannerController_%1%_%2%.log") % ::GetCurrentProcessId() % boost::lexical_cast<std::string>(boost::posix_time::second_clock::universal_time()));
		}
	} /// end unnamed namespace 

	
	
	namespace UseCase {
		
	int HiLevelScannerUseCase1::Run()
	{
		HiLevelScannerUseCase1 self;
		return self._Run();
	}

	HiLevelScannerUseCase1::HiLevelScannerUseCase1()
		: m_Controller(
			boost::assign::list_of
				(HKEY_CURRENT_USER)
				(HKEY_LOCAL_MACHINE)
				(HKEY_USERS)
				(HKEY_CLASSES_ROOT)
				(HKEY_CURRENT_CONFIG) /// этого хватит чтобы посмотреть на принцип работы... но все ветки лучше не перебирать - консоль загнётся! если перебирать всё то надо в файл или на форму выводить!
			, 5 /// это кол-во записей показываемых на экран за 1 раз
		)
		, m_Stoped(false)
	{
		m_ConnectionStore.emplace_back(m_Controller.AttachOnScanStartSignal(boost::bind(&HiLevelScannerUseCase1::_OnScanStart, this)));
		m_ConnectionStore.emplace_back(m_Controller.AttachOnScanEndSignal(boost::bind(&HiLevelScannerUseCase1::_OnScanEnd, this, _1)));
		m_ConnectionStore.emplace_back(m_Controller.AttachNextScanResultsCompleteSignal(boost::bind(&HiLevelScannerUseCase1::_OnNextScanResultsComplete, this, _1)));
	}

	int HiLevelScannerUseCase1::_Run()
	{
		m_Controller.ScanRegistryAssync();

		while (m_Queue.empty())
		{
			std::unique_lock<std::mutex> lock(m_Access);
			m_Condition.wait(lock);
		}

		volatile bool waitForStop(false);
		while (!m_Queue.empty())
		{
			if (waitForStop)
			{
				std::cout << "Wait for stop scanner thread." << std::endl;

				std::unique_lock<std::mutex> lock(m_Access);
				while (!m_Stoped)
					m_Condition.wait(lock);

				std::cout << "\n *** End *** \n" << std::endl;

				break;
			}
			else if (m_Stoped && m_StopLasyReporter)
			{
				m_StopLasyReporter();
				m_StopLasyReporter = StopLasyReporter_t();
			}
			else
			{
				std::cout << "Show next results set? y/n" << std::endl;

				char answer = 0;
				std::cin >> answer;
				std::cin.get();

				switch (answer)
				{
				case 'y':
				{
					HiLevelScannerController::ScanInfoStorePtr_t store;
					{
						std::unique_lock<std::mutex> const lock(m_Access);

						store = m_Queue.front();
						m_Queue.pop_front();
					}

					assert(store && "Bad data!");

					try
					{							
						std::wstring infoStr;
						for (auto currentScanInfo : *store)
							infoStr += ToWideString(*currentScanInfo) + L'\n';
						
						_File() << Details::StringCnv::w2a(infoStr) << std::endl;
					}
					catch (...) {}
					break;
				}
				case'n':
				{
					assert(waitForStop == false && "Bad logic!");
					if (!waitForStop)
					{
						waitForStop = true;
						m_Controller.StopScan();
					}
					break;
				}
				default:
					std::cerr << "Bad input!\n";
					break;
				}
			}
		}

		std::cout << "*** END OF USE CASE ***" << std::endl;

		::getchar();

		return 0;
	}

	void HiLevelScannerUseCase1::_OnScanStart()
	{
		std::cout << "HiLevelScannerUseCase1 is started." << std::endl;
		std::cout << "Wait for first results..." << std::endl;
	}

	void HiLevelScannerUseCase1::_OnScanEnd(bool aborted)
	{
		m_Stoped = true;

		m_ConnectionStore.clear();

		m_StopLasyReporter = [aborted]() { 
			std::cout << boost::str(boost::format("HiLevelScannerUseCase1 is %1%.") % (aborted ? "aborted" : "complete")) << std::endl;
		};

		m_Condition.notify_one();
	}

	void HiLevelScannerUseCase1::_OnNextScanResultsComplete(HiLevelScannerController::ScanInfoStorePtr_t store)
	{
		std::unique_lock<std::mutex> const lock(m_Access);

		assert(store && !store->empty() && "Bad params.");

#if defined(_DEBUG)
		for (size_t i = 0; i < store->size(); ++i)
			assert(store->at(i) && store->at(i)->data.is_initialized());
#endif /// _DEBUG

		m_Queue.push_back(store);

		m_Condition.notify_one();
	}

	std::ofstream& HiLevelScannerUseCase1::_File()
	{
		if (!m_File.is_open())
		{
			m_File.open(m_FilePath);
			m_File.unsetf(std::ios::skipws);
		}
	}
				
}} /// end namespace RegistryScanner::UseCase
