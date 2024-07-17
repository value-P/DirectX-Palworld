#pragma once

namespace Engine
{
	class ENGINE_DLL CEffectShader final
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CEffectShader() = default;

		virtual ~CEffectShader() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CEffectShader> Create(const ComPtr<ID3DX11Effect>& _cpEffect, const std::vector<ComPtr<ID3D11InputLayout>>& _vecInputLayouts, const ComPtr<ID3D11Device>& _cpDevice, const ComPtr<ID3D11DeviceContext>& _cpDeviceContext);

		HRESULT Initialize(const ComPtr<ID3DX11Effect>& _cpEffect, const std::vector<ComPtr<ID3D11InputLayout>>& _vecInputLayouts, const ComPtr<ID3D11Device>& _cpDevice, const ComPtr<ID3D11DeviceContext>& _cpDeviceContext);

		void Release();

		/********************
			Methods
		********************/
	public:
		HRESULT BeginPass(uint32 _iPassIndex, uint32 _iTechniqueIndex = 0);

		ComPtr<ID3DX11EffectVariable> GetRawVariableByName(const std::string& _strConstantName);
		
		ComPtr<ID3DX11EffectMatrixVariable> GetMatrixVariableByName(const std::string& _strConstantName);
		
		ComPtr<ID3DX11EffectShaderResourceVariable> GetShaderResourceVariableByName(const std::string& _strConstantName);

		/********************
			Data Members
		********************/
	private:
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

	private:
		ComPtr<ID3DX11Effect> m_cpEffect = nullptr;

		std::vector<ComPtr<ID3D11InputLayout>> m_vecInputLayouts;
	};
}