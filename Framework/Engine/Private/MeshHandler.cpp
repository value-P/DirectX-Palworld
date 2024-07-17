#include "pch.h"
#include "MeshHandler.h"

#include "EngineModule.h"
#include "DeviceManager.h"
#include "RenderManager.h"
#include "ShaderHandler.h"
#include "ThreadManager.h"

#include "MeshBuffer.h"
#include "EffectShader.h"

IMPLEMENT_SINGLETON(CMeshHandler)

HRESULT Engine::CMeshHandler::Initialize()
{
	shared_ptr<CDeviceManager> spDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_cpDevice = spDeviceManager->GetDevice();
	m_cpDeviceContext = spDeviceManager->GetDeviceContext();
	m_cpDeferredDeviceContext = spDeviceManager->GetDeferredDeviceContext();
	m_cpMultithread = spDeviceManager->GetMultithread();

	m_wpThreadManager = CEngineModule::GetInstance()->GetThreadManager();
	m_wpRenderManager = CEngineModule::GetInstance()->GetRenderManager();
	m_wpShaderHandler = CShaderHandler::GetInstance();

	return S_OK;
}

void Engine::CMeshHandler::Release()
{
	for (auto& pair : m_umapMeshes) { pair.second = nullptr; }
	m_umapMeshes.clear();

	for (auto& pair : m_umapVaraible_MeshInfo) { pair.second = nullptr; }
	m_umapVaraible_MeshInfo.clear();

	for (auto& pair : m_umapVaraible_Textures) { pair.second = nullptr; }
	m_umapVaraible_Textures.clear();

	m_cpDevice = nullptr;
	m_cpDeviceContext = nullptr;

	m_spInstance = nullptr;
}

HRESULT Engine::CMeshHandler::LoadMeshBufferFromFile(const wstring& _wstrModelFilePath, ifstream& _ifFile, const wstring& _wstrMeshName, const Matrix& _matPivot, EModelType _eModelType, EMeshManageType _eMeshType)
{
	shared_ptr<CMeshBuffer> spMeshBuffer = CMeshBuffer::Create(_wstrModelFilePath, _ifFile, _matPivot, _eModelType, _eMeshType);
	spMeshBuffer->SetMeshName(_wstrMeshName);
	m_umapMeshes.insert({ _wstrMeshName, spMeshBuffer });

	if (spMeshBuffer == nullptr) { return E_FAIL; }


	for (auto& spDst : m_umapMeshRequests[_wstrMeshName])
	{
		*spDst = m_umapMeshes[_wstrMeshName];
	}
	m_umapMeshRequests.erase(_wstrMeshName);
	return S_OK;
}

void Engine::CMeshHandler::RequestMeshBuffer(const wstring& _wstrModelFilePath, ifstream& _ifFile, shared_ptr<CMeshBuffer>* _spDst, const wstring& _wstrMeshName, const Matrix& _matPivot, EModelType _eModelType, EMeshManageType _eMeshType)
{
	// 이미 로드된 메쉬인지 검사
	if (m_umapMeshes.contains(_wstrMeshName))
	{
		*_spDst = m_umapMeshes[_wstrMeshName];
	}
	// 로딩 중인 메쉬인지 검사
	else if (m_umapMeshRequests.contains(_wstrMeshName))
	{
		m_umapMeshRequests[_wstrMeshName].push_back(_spDst);
	}
	// 새로 로딩해야 될 때
	else
	{
		m_umapMeshRequests[_wstrMeshName].push_back(_spDst);
		ENSURE(LoadMeshBufferFromFile(_wstrModelFilePath, _ifFile, _wstrMeshName, _matPivot, _eModelType, _eMeshType));
	}
}

void Engine::CMeshHandler::RegisterRender(ERenderType _eRenderType, EShaderType _eShaderType, weak_ptr<CMeshBuffer> _spMeshBuffer)
{
	m_umapReservedRenders[_eRenderType][_eShaderType].push_back(_spMeshBuffer);

	// 임시
	if (!m_umapShaders.contains(EShaderType::DEFAULT_INSTANCE))
	{
		m_umapShaders[EShaderType::DEFAULT_INSTANCE] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_DefaultInstance_PNT2TRULP");
		m_umapVaraible_MeshInfo[EShaderType::DEFAULT_INSTANCE] = m_umapShaders[EShaderType::DEFAULT_INSTANCE]->GetRawVariableByName("g_tMeshInfo");
		m_umapVaraible_Textures[EShaderType::DEFAULT_INSTANCE] = m_umapShaders[EShaderType::DEFAULT_INSTANCE]->GetShaderResourceVariableByName("g_Textures");

		m_umapShaders[EShaderType::PALLIT_TOP_INSTANCE] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_PalLitTopInstance_PNT2TRULP");
		m_umapVaraible_MeshInfo[EShaderType::PALLIT_TOP_INSTANCE] = m_umapShaders[EShaderType::PALLIT_TOP_INSTANCE]->GetRawVariableByName("g_tMeshInfo");
		m_umapVaraible_Textures[EShaderType::PALLIT_TOP_INSTANCE] = m_umapShaders[EShaderType::PALLIT_TOP_INSTANCE]->GetShaderResourceVariableByName("g_Textures");

		m_umapShaders[EShaderType::EXTERN_INSTANCE] = m_wpShaderHandler.lock()->FindEffectShader(L"FX_ExternInstance_PNT2TRULP");
		m_umapVaraible_MeshInfo[EShaderType::EXTERN_INSTANCE] = m_umapShaders[EShaderType::EXTERN_INSTANCE]->GetRawVariableByName("g_tMeshInfo");
		m_umapVaraible_Textures[EShaderType::EXTERN_INSTANCE] = m_umapShaders[EShaderType::EXTERN_INSTANCE]->GetShaderResourceVariableByName("g_Textures");
	}
}

void Engine::CMeshHandler::RefreshInstances()
{
	for (auto& Pair1 : m_umapReservedRenders)
	{
		for (auto& Pair2 : Pair1.second)
		{
			for (auto& Pair3 : Pair2.second)
			{
				Pair3.lock()->RefreshInstances();
			}
		}
	}
}

void Engine::CMeshHandler::RenderInstances(ERenderType _eRenderType, int32 _iShaderPath)
{
	for (auto& Pair : m_umapReservedRenders[_eRenderType])
	{
		if (m_umapShaders.contains(Pair.first))
		{
			for (auto& wpMeshBuffer : Pair.second)
			{
				wpMeshBuffer.lock()->BindVariable(m_umapVaraible_MeshInfo[Pair.first], m_umapVaraible_Textures[Pair.first]);
				m_umapShaders[Pair.first]->BeginPass(_iShaderPath);
				wpMeshBuffer.lock()->Render_Instance(_eRenderType);
			}
		}
	}
}