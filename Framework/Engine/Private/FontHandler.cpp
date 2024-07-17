#include "pch.h"
#include "FontHandler.h"

#include "EngineModule.h"
#include "ThreadManager.h"

#include "Font.h"

IMPLEMENT_SINGLETON(CFontHandler)

HRESULT Engine::CFontHandler::Initialize()
{
	m_wpThreadManager = CEngineModule::GetInstance()->GetThreadManager();
	return S_OK;
}

void Engine::CFontHandler::Release()
{
	for (auto& pair : m_umapFonts)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapFonts.clear();

	m_spInstance = nullptr;
}

HRESULT Engine::CFontHandler::LoadFontFromFile()
{
	for (const std::filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrFontFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		// spritefont 파일이 아닌 경우
		if (fsEntry.path().extension().generic_string() != ".spritefont" && fsEntry.path().extension().generic_string() != ".SPRITEFONT") { continue; }

		ENSUREF(LoadFont(fsEntry), L"Failed to load font.");
	}
	return S_OK;
}

HRESULT Engine::CFontHandler::LoadFont(const std::filesystem::directory_entry& _fsEntry)
{
	// 파일의 이름을 구합니다.
	wstring wstrFileName = _fsEntry.path().stem().generic_wstring();
	wstring wstrFilePath = _fsEntry.path().generic_wstring();

	std::shared_ptr<CFont> spFont = CFont::Create(wstrFilePath);
	return AddFont(wstrFileName, spFont);
}

HRESULT Engine::CFontHandler::AddFont(const std::wstring& _wstrFontName, const std::shared_ptr<CFont>& _spFont)
{
	if (m_umapFonts.contains(_wstrFontName)) { return E_FAIL; }

	m_umapFonts.emplace(_wstrFontName, _spFont);
	return S_OK;
}

std::shared_ptr<CFont> Engine::CFontHandler::FindFont(const std::wstring& _wstrFontName)
{
	if (!m_umapFonts.contains(_wstrFontName)) { return nullptr; }

	return m_umapFonts[_wstrFontName];
}
