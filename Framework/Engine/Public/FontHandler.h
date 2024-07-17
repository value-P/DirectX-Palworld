#pragma once

namespace Engine
{
	class CThreadManager;

	class CFont;

	class ENGINE_DLL CFontHandler final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CFontHandler)
		
	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		void Release();
		
	/********************
		Methods
	********************/
	public:
		/*
		* @detail
		* ��Ʈ ���ϵ��� ����Ǿ� �ִ� ���� ��ηκ��� ��Ʈ ��ü�� �ε��մϴ�.
		*/
		HRESULT LoadFontFromFile();

		HRESULT AddFont(const std::wstring& _wstrFontName, const std::shared_ptr<CFont>& _spFont);

		std::shared_ptr<CFont> FindFont(const std::wstring& _wstrFontName);

	private:
		HRESULT LoadFont(const std::filesystem::directory_entry& _fsEntry);
	/********************
		Getter/Setter
	********************/
	public:
		const std::unordered_map<std::wstring, std::shared_ptr<CFont>>& GetFonts() { return m_umapFonts; }

	/********************
		Data Members
	********************/
	private:
		// ��Ʈ��
		std::unordered_map<std::wstring, std::shared_ptr<CFont>> m_umapFonts;

		// ��Ʈ ���� ���
		std::wstring m_wstrFontFilePath = L"../../Resource/Fonts/";

	private:
		std::weak_ptr<CThreadManager> m_wpThreadManager;
	};
}
