#pragma once
#include "json.h"
namespace Engine
{
	class CTexture;

	class CMaterial final
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		CMaterial() = default;
		virtual ~CMaterial() = default;

		/********************
			Events
		********************/
	public:
		static shared_ptr<CMaterial> Create(wstring _wstrModelFilePath, ifstream& _ifFile, EModelType _eModelType, EMeshManageType _eMeshType);

		virtual HRESULT Initialize(wstring _wstrModelFilePath, ifstream& _ifFile, EModelType _eModelType, EMeshManageType _eMeshType);

		void Release();

		/********************
			Get / Set
		********************/
	public:
		EShaderType GetShaderType() { return m_eShaderType; }
		ComPtr<ID3D11ShaderResourceView> GetSRV(const wstring& _wstrUsage);
		int32 GetInt(const wstring& _wstrUsage);
		float GetFloat(const wstring& _wstrUsage);
		Vector2 GetVector2(const wstring& _wstrUsage);
		Vector3 GetVector3(const wstring& _wstrUsage);
		Vector4 GetVector4(const wstring& _wstrUsage);
		Matrix GetMatrix(const wstring& _wstrUsage);

		void SetShaderType(EShaderType _eShaderType) { m_eShaderType = _eShaderType; m_bUpdated = true; }
		void SetSRV(wstring _wstrUsage, ComPtr<ID3D11ShaderResourceView> _cpSRV);
		void SetInt(wstring _wstrUsage, int32 _iVal)
		{
			m_umapInts[_wstrUsage] = _iVal;
			m_bUpdated = true;
		}
		void SetFloat(wstring _wstrUsage, float _fVal) { m_umapFloats[_wstrUsage] = _fVal; m_bUpdated = true; }
		void SetVector2(wstring _wstrUsage, Vector2 _vVal) { m_umapVector2s[_wstrUsage] = _vVal; m_bUpdated = true; }
		void SetVector3(wstring _wstrUsage, Vector3 _vVal) { m_umapVector3s[_wstrUsage] = _vVal; m_bUpdated = true; }
		void SetVector4(wstring _wstrUsage, Vector4 _vVal) { m_umapVector4s[_wstrUsage] = _vVal; m_bUpdated = true; }
		void SetMatrix(wstring _wstrUsage, Matrix _matVal) { m_umapmatrices[_wstrUsage] = _matVal; m_bUpdated = true; }

		bool GetUpdated() { return m_bUpdated; }
		void ResetUpdated() { m_bUpdated = false; }
		const wstring& GetName() { return m_wstrMaterialName; }

		/********************
			Data Members
		********************/
	private:
		wstring m_wstrMaterialName = L"";
		EShaderType m_eShaderType = EShaderType::ENUM_END;
		unordered_map<wstring, ComPtr<ID3D11ShaderResourceView>> m_umapSRVs;
		unordered_map<wstring, int32> m_umapInts;
		unordered_map<wstring, float> m_umapFloats;
		unordered_map<wstring, Vector2> m_umapVector2s;
		unordered_map<wstring, Vector3> m_umapVector3s;
		unordered_map<wstring, Vector4> m_umapVector4s;
		unordered_map<wstring, Matrix> m_umapmatrices;
		bool m_bUpdated = false;
	};
}