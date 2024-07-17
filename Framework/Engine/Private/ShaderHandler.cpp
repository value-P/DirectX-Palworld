#include "pch.h"
#include "ShaderHandler.h"

#include "EngineModule.h"
#include "DeviceManager.h"
#include "ThreadManager.h"

#include "EffectShader.h"

IMPLEMENT_SINGLETON(CShaderHandler)

HRESULT Engine::CShaderHandler::Initialize()
{
	shared_ptr<CDeviceManager> spDeviceManager = CEngineModule::GetInstance()->GetDeviceManager();
	m_cpDevice = spDeviceManager->GetDevice();
	m_cpDeviceContext = spDeviceManager->GetDeviceContext();

	m_wpThreadManager = CEngineModule::GetInstance()->GetThreadManager();
	return S_OK;
}

void Engine::CShaderHandler::Release()
{
	for (auto& pair : m_umapVariables_Frame)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapVariables_Frame.clear();

	for (auto& pair : m_umapVariables_Camera)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapVariables_Camera.clear();

	// 이펙트 셰이더를 해제합니다.
	for (auto& pair : m_umapEffectShaders)
	{
		pair.second->Release();
		pair.second = nullptr;
	}
	m_umapEffectShaders.clear();

	m_spBasicEffect = nullptr;
	m_spPrimitiveBatch = nullptr;
	m_cpDebugDrawInputLayout = nullptr;

	m_cpDevice = nullptr;
	m_cpDeviceContext = nullptr;

	m_spInstance = nullptr;
}

HRESULT Engine::CShaderHandler::LoadEffectShaderFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrShaderFilePath))
	{
		// 파일이 아닌 경우, 건너뜁니다.
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		ENSUREF(LoadEffectShader(fsEntry), L"Failed to load effect shader.");
	}

	// 디버그 드로우를 로드합니다.
	ENSUREF(LoadBasicEffect(), L"Failed to load basic effect.");

	return S_OK;
}

HRESULT Engine::CShaderHandler::LoadEffectShader(const filesystem::directory_entry& _fsEntry)
{
	uint32 iCompileFlag = 0;

#ifdef _DEBUG
	iCompileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iCompileFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	wstring wstrShaderFileName = _fsEntry.path().stem().generic_wstring();
	wstring wstrShaderFilePath = _fsEntry.path().generic_wstring();

	int32 iCursor = static_cast<int32>(wstrShaderFileName.rfind(L"_"));
	CHECKF(iCursor != -1, L"Shader file doesn't have input layout postfix.");

	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs = nullptr;
	uint32 iNumElementDescs = 0;

	wstring wstrPostfix = wstrShaderFileName.substr(iCursor + 1);

	// POSITION
	if (wstrPostfix == L"P")
	{
		pInputElementDescs = FPosition::m_tInputElementDescs;
		iNumElementDescs = FPosition::iNumInputElementDescs;
	}
	// POSITION, SIZE
	else if (wstrPostfix == L"PS")
	{
		pInputElementDescs = FPositionSize::m_tInputElementDescs;
		iNumElementDescs = FPositionSize::iNumInputElementDescs;
	}
	// POSITION, COLOR
	else if (wstrPostfix == L"PC")
	{
		pInputElementDescs = FPositionColor::m_tInputElementDescs;
		iNumElementDescs = FPositionColor::iNumInputElementDescs;
	}
	// POSITION, COLOR
	else if (wstrPostfix == L"PSC")
	{
		pInputElementDescs = FPositionSizeColor::m_tInputElementDescs;
		iNumElementDescs = FPositionSizeColor::iNumInputElementDescs;
	}
	// POSITION, TEXCOORD2
	else if (wstrPostfix == L"PT2")
	{
		pInputElementDescs = FPositionTexCoord2::m_tInputElementDescs;
		iNumElementDescs = FPositionTexCoord2::iNumInputElementDescs;
	}
	// POSITION, TEXCOORD3
	else if (wstrPostfix == L"PT3")
	{
		pInputElementDescs = FPositionTexCoord3::m_tInputElementDescs;
		iNumElementDescs = FPositionTexCoord3::iNumInputElementDescs;
	}
	// POSITION, COLOR, TEXCOORD2
	else if (wstrPostfix == L"PCT2")
	{
		pInputElementDescs = FPositionColorTexCoord2::m_tInputElementDescs;
		iNumElementDescs = FPositionColorTexCoord2::iNumInputElementDescs;
	}
	// POSITION, NORMAL, TEXCOORD2, TANGENT
	else if (wstrPostfix == L"PNT2T")
	{
		pInputElementDescs = FPositionNormalTexCoord2Tangent::m_tInputElementDescs;
		iNumElementDescs = FPositionNormalTexCoord2Tangent::iNumInputElementDescs;
	}
	// POSITION, NORMAL, TEXCOORD2, TANGENT
	else if (wstrPostfix == L"PNT2TRULP")
	{
		pInputElementDescs = FNonAnimMeshInstance::m_tInputElementDescs;
		iNumElementDescs = FNonAnimMeshInstance::iNumInputElementDescs;
	}
	// POSITION, NORMAL, TEXCOORD2, TANGENT, BLEND
	else if (wstrPostfix == L"PNT2TBIBW")
	{
		pInputElementDescs = FPositionNormalTexCoord2TangentBlendIndexBlendWeight::m_tInputElementDescs;
		iNumElementDescs = FPositionNormalTexCoord2TangentBlendIndexBlendWeight::iNumInputElementDescs;
	}
	// POSITION, NORMAL, TEXCOORD2, TEXCOORD2, TANGENT
	else if (wstrPostfix == L"PNT2T2T")
	{
		pInputElementDescs = FPositionNormalTexCoord2TexCoord2Tangent::m_tInputElementDescs;
		iNumElementDescs = FPositionNormalTexCoord2TexCoord2Tangent::iNumInputElementDescs;
	}
	// PointInstance
	else if (wstrPostfix == L"PRULPC")
	{
		pInputElementDescs = FPointInstance::m_tInputElementDescs;
		iNumElementDescs = FPointInstance::iNumInputElementDescs;
	}
	// EffectMeshInstance
	else if (wstrPostfix == L"PNT2TRULPP")
	{
		pInputElementDescs = FEffectMeshInstance::m_tInputElementDescs;
		iNumElementDescs = FEffectMeshInstance::iNumInputElementDescs;
	}
	// MeshInstance
	else if (wstrPostfix == L"PNT2TRULPI")
	{
		pInputElementDescs = FMeshInstance::m_tInputElementDescs;
		iNumElementDescs = FMeshInstance::iNumInputElementDescs;
	}
	else if (wstrPostfix == L"PSC")
	{
		pInputElementDescs = FPositionSize::m_tInputElementDescs;
		iNumElementDescs = FPositionSize::iNumInputElementDescs;
	}
	// Defines
	else if (wstrPostfix == L"Defines")
	{
		return S_OK;
	}
	else
	{
		CHECKF(false, L"Shader file doesn't have input layout postfix.");
	}

	ID3DBlob* pBlop;
	ComPtr<ID3DX11Effect> cpEffect = nullptr;
	D3DX11CompileEffectFromFile(wstrShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iCompileFlag, 0, m_cpDevice.Get(), cpEffect.GetAddressOf(), &pBlop);
	char* szTest = (char*)pBlop->GetBufferPointer();

	D3DX11_EFFECT_DESC tEffectDesc{};
	cpEffect->GetDesc(&tEffectDesc);

	vector<ComPtr<ID3D11InputLayout>> vecInputLayouts;

	for (uint32 iOuterIndex = 0; iOuterIndex < tEffectDesc.Techniques; ++iOuterIndex)
	{
		ComPtr<ID3DX11EffectTechnique> cpTechnique = cpEffect->GetTechniqueByIndex(iOuterIndex);
		CHECKF(cpTechnique != nullptr, L"Technique is nullptr.");

		D3DX11_TECHNIQUE_DESC tTechniqueDesc{};
		cpTechnique->GetDesc(&tTechniqueDesc);

		for (uint32 iInnerIndex = 0; iInnerIndex < tTechniqueDesc.Passes; ++iInnerIndex)
		{
			ComPtr<ID3DX11EffectPass> cpPass = cpTechnique->GetPassByIndex(iInnerIndex);
			CHECKF(cpPass != nullptr, L"Pass is nullptr.");

			D3DX11_PASS_DESC tPassDesc{};
			cpPass->GetDesc(&tPassDesc);

			ComPtr<ID3D11InputLayout> cpInputLayout = nullptr;
			ENSURE(m_cpDevice->CreateInputLayout(pInputElementDescs, iNumElementDescs, tPassDesc.pIAInputSignature, tPassDesc.IAInputSignatureSize, cpInputLayout.GetAddressOf()));

			vecInputLayouts.push_back(cpInputLayout);
		}
	}

	std::shared_ptr<CEffectShader> spEffectShader = CEffectShader::Create(cpEffect, vecInputLayouts, m_cpDevice, m_cpDeviceContext);

	return AddEffectShader(wstrShaderFileName, spEffectShader);
}

HRESULT Engine::CShaderHandler::LoadBasicEffect()
{
	m_spPrimitiveBatch = make_shared<PrimitiveBatch<VertexPositionColor>>(m_cpDeviceContext.Get());
	m_spBasicEffect = make_shared<BasicEffect>(m_cpDevice.Get());
	m_spBasicEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t iByteCodeLength = 0;
	m_spBasicEffect->GetVertexShaderBytecode(&pShaderByteCode, &iByteCodeLength);

	ENSUREF(m_cpDevice->CreateInputLayout
	(
		VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		pShaderByteCode,
		iByteCodeLength,
		m_cpDebugDrawInputLayout.GetAddressOf()),
		L"Failed to create input layout."
	);
	return S_OK;
}

void Engine::CShaderHandler::BindFrameInfo(const FFrameInfo& _tFrameInfo)
{
	for (auto& Pair : m_umapEffectShaders)
	{
		m_umapVariables_Frame[Pair.first]->SetRawValue(&_tFrameInfo, 0, sizeof(_tFrameInfo));
	}
}

void Engine::CShaderHandler::BindCameraInfo(const FCameraInfo& _tCameraInfo)
{
	for (auto& Pair : m_umapEffectShaders)
	{
		m_umapVariables_Camera[Pair.first]->SetRawValue(&_tCameraInfo, 0, sizeof(_tCameraInfo));
	}
}

HRESULT Engine::CShaderHandler::AddEffectShader(const std::wstring& _wstrEffectShaderName, const std::shared_ptr<CEffectShader>& _spEffectShader)
{
	if (m_umapEffectShaders.contains(_wstrEffectShaderName)) { return E_FAIL; }

	m_umapEffectShaders.emplace(_wstrEffectShaderName, _spEffectShader);
	m_umapVariables_Frame.emplace(_wstrEffectShaderName, _spEffectShader->GetRawVariableByName("g_tFrame"));
	m_umapVariables_Camera.emplace(_wstrEffectShaderName, _spEffectShader->GetRawVariableByName("g_tCamera"));
	return S_OK;
}

std::shared_ptr<CEffectShader> Engine::CShaderHandler::FindEffectShader(const std::wstring& _wstrEffectShaderName)
{
	scoped_lock slLock(m_mtMutex);

	if (!m_umapEffectShaders.contains(_wstrEffectShaderName)) { return nullptr; }

	return m_umapEffectShaders[_wstrEffectShaderName];
}