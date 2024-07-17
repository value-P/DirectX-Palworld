#pragma once
#include <codecvt>
#include <locale>
namespace Engine
{
	class ENGINE_DLL CStringUtility final
	{
	public:
		/*
		* @detail:	wstring 타입의 문자열을 전달받아 string 타입으로 변환합니다.
		*/
		static std::string ToString(const std::wstring& _wstrWstring)
		{
			std::vector<char> Bufer(_wstrWstring.size());
			std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(_wstrWstring.data(), _wstrWstring.data() + _wstrWstring.size(), '?', Bufer.data());
			return std::string(Bufer.data(), Bufer.size());
		}

		/*
		* @detail:	string 타입의 문자열을 전달받아 wstring 타입으로 변환합니다.
		*/
		static std::wstring ToWideString(const std::string& _strString)
		{
			std::vector<wchar_t> buf(_strString.size());
			std::use_facet<std::ctype<wchar_t>>(std::locale()).widen(_strString.data(),
				_strString.data() + _strString.size(),
				buf.data());
			return std::wstring(buf.data(), buf.size());
		}

		//static std::string ToStringKR(const std::wstring& _wstrWstring)
		//{
		////int size_needed = WideCharToMultiByte(CP_UTF8, 0, _wstrWstring.c_str(), -1, NULL, 0, NULL, NULL);
		//	//std::string mbStr(size_needed, '\0');
		//	//WideCharToMultiByte(CP_UTF8, 0, _wstrWstring.c_str(), -1, &mbStr[0], size_needed -1, NULL, NULL);
		//	//return mbStr;
		//	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.to_bytes(_wstrWstring);
		//}

		//static std::wstring ToWideStringKR(const std::string& _strString)
		//{
		//	//wstring unicode_string = CA2W(_strString.c_str()); // multi_byte -> unicode(utf_16)
		//	//return CW2A(unicode_string.c_str(), CP_UTF8); // unicode(utf_16) -> utf_8


		//	int szLength = MultiByteToWideChar(CP_UTF8, 0, _strString.c_str(), -1, NULL, 0);
		//	wchar_t Wszstring[MAX_PATH] = L"";
		//	MultiByteToWideChar(CP_UTF8, 0, _strString.c_str(), -1, Wszstring, szLength);
		//	std::wstring wstrWstring = std::wstring(Wszstring, Wszstring + wcslen(Wszstring));
		//	return wstrWstring;
		//}
	};
}
