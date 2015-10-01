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
#include "Handle2Path.h"
#include "StringCnv.h"
#include "FileHelpers.h"
#include "TimeStamp.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>

namespace RegistryScanner {

	HiLevelScannerController::HiLevelScannerController(Descriptors_t&& params, size_t chunkSize)
		: m_HkeyStore(std::forward<Descriptors_t>(params))
		, m_ChunkSize(chunkSize)
		, m_Out(nullptr)
	{
	}

	HiLevelScannerController::~HiLevelScannerController() {
		_StopScan(true, false);
	}

	void HiLevelScannerController::SetOutput(std::ostream& os) {
		m_Out = std::addressof(os);
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
		return m_Scanner->AttachOnErrorFoundSignal(slot);
	}

	HiLevelScannerController::Connection_t HiLevelScannerController::AttachOnOperationSuccessSignal(OnOperationSuccess_t::slot_type slot) {
		return m_Scanner->AttachOnOperationSuccessSignal(slot);
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

	namespace {

		boost::filesystem::path _GenScannerFileName(HKEY root) {
			return boost::str(boost::format("HiLevelScanner_%1%_%2%_%3%.log")
				% Details::StringCnv::w2a(Details::Handle2Path(root))
				% ::GetCurrentProcessId()
				% Details::TimeStamp()
				);
		}

		class _ScannerObserver
		{
		public:
			typedef HiLevelScannerController::ConnectionStore_t ConnectionStore_t;
			typedef HiLevelScannerController::Connection_t Connection_t;
			typedef IScanerDispatcher::OnPathFoundSignal_t OnPathFoundSignal_t;

		public:
			_ScannerObserver(IScanerDispatcher& disp, OnPathFoundSignal_t::slot_type&& slot, boost::filesystem::path&& filePath)
				: m_FilePath(std::forward<boost::filesystem::path>(filePath))
				, m_ConnectionStore(_Build(disp, std::forward<OnPathFoundSignal_t::slot_type>(slot)))
			{
				assert(!m_FilePath.empty());
			}

			~_ScannerObserver() {
				Details::FileHelpers::CloseFile(m_File);
			}

		private:
			ConnectionStore_t _Build(IScanerDispatcher& disp, OnPathFoundSignal_t::slot_type&& slot)
			{
				ConnectionStore_t connectionStore;

				connectionStore.emplace_back(
					disp.AttachOnPathFoundSignal([this](ScanInfoPtr_t scanInfo) {
					m_OnPathFoundSignal(scanInfo);
				})
					);

				connectionStore.emplace_back(
					disp.AttachOnErrorFoundSignal([this](LONG erroCode, std::wstring message) {
					_OnErrorFound(erroCode, message);
				})
					);

				connectionStore.emplace_back(
					disp.AttachOnOperationSuccessSignal([this](std::wstring message) {
					_OnOperationSuccess(message);
				})
					);

				connectionStore.emplace_back(
					disp.AttachOnInformationSignal([this](std::wstring message) {
					_OnInformation(message);
				})
					);

				connectionStore.emplace_back(m_OnPathFoundSignal.connect(slot));

				return connectionStore;
			}

		private:
			void _OnErrorFound(LONG erroCode, std::wstring message) {
				_File() << boost::str(boost::format("\n*** Error found. Reason: error code: %1%, message: %2% \n") % erroCode % Details::StringCnv::w2a(message)) << std::endl;
			}

			void _OnOperationSuccess(std::wstring message) {
				_File() << boost::str(boost::format("\n*** Operation success. Info: %1% \n") % Details::StringCnv::w2a(message)) << std::endl;
			}

			void _OnInformation(std::wstring message) {
				_File() << boost::str(boost::format("\n*** Information: %1% \n") % Details::StringCnv::w2a(message)) << std::endl;
			}

			std::ofstream& _File() {
				return Details::FileHelpers::LazyFile(m_File, m_FilePath);
			}

		private:
			boost::filesystem::path m_FilePath;
			boost::filesystem::ofstream m_File;
			OnPathFoundSignal_t m_OnPathFoundSignal;
			ConnectionStore_t m_ConnectionStore;
		};

	} /// end unnamed namespace

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

			_DoScan(currentHkey);

			m_Scanner.reset();
		}

		_PostScan();
		m_OnScanEndSignal(false);
	}


	void HiLevelScannerController::_DoScan(HKEY hkey)
	{
		_ScannerObserver const observeer(*m_Scanner
			, boost::bind(&HiLevelScannerController::_OnPathFound, this, _1)
			, _GenScannerFileName(hkey)
			);

		m_Scanner->Scan();
	}

	void HiLevelScannerController::_PostScan()
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

		if (m_Out)
			*m_Out << "\n*** ScanInfo is received successfully.\n" << std::endl;

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
