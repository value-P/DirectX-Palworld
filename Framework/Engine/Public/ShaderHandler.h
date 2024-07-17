#pragma once

namespace Engine
{
	class CThreadManager;
	class CEffectShader;

	class ENGINE_DLL CShaderHandler final
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		DECLARE_SINGLETON(CShaderHandler)

		/********************
			Events
		********************/
	public:
		HRESULT Initialize();

		void Release();

		/******************
			Methods
		******************/
	public:
		HRESULT LoadEffectShaderFromFile();

		HRESULT AddEffectShader(const std::wstring& _wstrEffectShaderName, const std::shared_ptr<CEffectShader>& _spEffectShader);

		std::shared_ptr<CEffectShader> FindEffectShader(const std::wstring& _wstrEffectShaderName);

	private:
		HRESULT LoadEffectShader(const filesystem::directory_entry& _fsEntry);

		HRESULT LoadBasicEffect();

		/********************
			Getter/Setter
		********************/
	public:
		const std::unordered_map<std::wstring, std::shared_ptr<CEffectShader>>& GetEffectShaders() { return m_umapEffectShaders; }

		std::shared_ptr<PrimitiveBatch<VertexPositionColor>> GetPrimitiveBatch() const { return m_spPrimitiveBatch; }

		std::shared_ptr<BasicEffect> GetBasicEffect() const { return m_spBasicEffect; }

		ComPtr<ID3D11InputLayout> GetDebugDrawInputLayout() const { return m_cpDebugDrawInputLayout; }

		void BindFrameInfo(const FFrameInfo& _tFrameInfo);
		void BindCameraInfo(const FCameraInfo& _tCameraInfo);

		/********************
			Data Members
		********************/
	private:
		// 이펙트 셰이더들
		std::unordered_map<std::wstring, std::shared_ptr<CEffectShader>> m_umapEffectShaders;

		// Bind Variables
		std::unordered_map<std::wstring, ComPtr<ID3DX11EffectVariable>> m_umapVariables_Frame;
		std::unordered_map<std::wstring, ComPtr<ID3DX11EffectVariable>> m_umapVariables_Camera;

		// 셰이더 파일 경로
		std::wstring m_wstrShaderFilePath = L"../../Resource/Shaders/";

	private:
		// 디바이스
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

	private:
		// 디버그 드로우
		std::shared_ptr<PrimitiveBatch<VertexPositionColor>> m_spPrimitiveBatch = nullptr;

		std::shared_ptr<BasicEffect> m_spBasicEffect = nullptr;

		ComPtr<ID3D11InputLayout> m_cpDebugDrawInputLayout = nullptr;

	private:
		// 스레드 매니저
		std::weak_ptr<CThreadManager> m_wpThreadManager;

		std::mutex m_mtMutex;
	};
}
