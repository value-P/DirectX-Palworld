#pragma once

namespace Engine
{
	class CThreadManager;
	class CMeshBuffer;
	class CEffectShader;
	class CShaderHandler;
	class CRenderManager;

	class ENGINE_DLL CMeshHandler final
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		DECLARE_SINGLETON(CMeshHandler)

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
		HRESULT LoadMeshBufferFromFile(const wstring& _wstrModelFilePath, ifstream& _ifFile, const wstring& _wstrMeshName, const Matrix& _matPivot, EModelType _eModelType, EMeshManageType _eMeshType);

		void RequestMeshBuffer(const wstring& _wstrModelFilePath, ifstream& _ifFile, shared_ptr<CMeshBuffer>* _spDst, const wstring& _wstrMeshName, const Matrix& _matPivot, EModelType _eModelType, EMeshManageType _eMeshType);

		void RegisterRender(ERenderType _eRenderType, EShaderType _eShaderType, weak_ptr<CMeshBuffer> _spMeshBuffer);

		void RefreshInstances();

		void RenderInstances(ERenderType _eRenderType, int32 _iShaderPath);

		/********************
			Getter/Setter
		********************/
	public:
		const std::unordered_map<std::wstring, shared_ptr<CMeshBuffer>>& GetMeshes() { return m_umapMeshes; }

		/********************
			Data Members
		********************/
	private:
		// 메쉬 목록
		std::unordered_map<wstring, shared_ptr<CMeshBuffer>>                                                    m_umapMeshes;
		std::unordered_map<wstring, vector<shared_ptr<CMeshBuffer>*>>                                           m_umapMeshRequests;
		std::unordered_map<EShaderType, shared_ptr<CEffectShader>>								                m_umapShaders;
		std::unordered_map<EShaderType, ComPtr<ID3DX11EffectVariable>>											m_umapVaraible_MeshInfo;
		std::unordered_map<EShaderType, ComPtr<ID3DX11EffectShaderResourceVariable>>							m_umapVaraible_Textures;
		std::unordered_map<ERenderType, unordered_map<EShaderType, vector<weak_ptr<CMeshBuffer>>>>              m_umapReservedRenders;

		// 메쉬 파일 경로
		std::wstring m_wstrMeshPath = L"../../Resource/Models/Meshes/";

	private:
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		ComPtr<ID3D11DeviceContext> m_cpDeferredDeviceContext = nullptr;

		ComPtr<ID3D11Multithread> m_cpMultithread = nullptr;

	private:
		std::weak_ptr<CThreadManager> m_wpThreadManager;
		std::weak_ptr<CRenderManager> m_wpRenderManager;
		std::weak_ptr<CShaderHandler> m_wpShaderHandler;

		std::mutex m_mtMutex;
	};
}