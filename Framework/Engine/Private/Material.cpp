#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include "TextureHandler.h"
#include "StringUtility.h"

shared_ptr<CMaterial> Engine::CMaterial::Create(wstring _wstrModelFilePath, ifstream& _ifFile, EModelType _eModelType, EMeshManageType _eMeshType)
{
	shared_ptr<CMaterial> spInstance = make_shared<CMaterial>();

	CHECKF(spInstance != nullptr, L"CMaterial Create Failed");
	spInstance->Initialize(_wstrModelFilePath, _ifFile, _eModelType, _eMeshType);

	return spInstance;
}

HRESULT Engine::CMaterial::Initialize(wstring _wstrModelFilePath, ifstream& _ifFile, EModelType _eModelType, EMeshManageType _eMeshType)
{
	int32 iDataTableNameLen = 0;
	wchar_t wszDataTableName[MAX_PATH] = {};
	_ifFile.read(reinterpret_cast<char*>(&iDataTableNameLen), sizeof(iDataTableNameLen));
	_ifFile.read(reinterpret_cast<char*>(const_cast<wchar_t*>(wszDataTableName)), iDataTableNameLen * sizeof(wchar_t));
	m_wstrMaterialName = wszDataTableName;

	int32 iShaderTagLen = 0;
	wchar_t wszShaderTag[MAX_PATH] = {};
	_ifFile.read(reinterpret_cast<char*>(&iShaderTagLen), sizeof(iShaderTagLen));
	_ifFile.read(reinterpret_cast<char*>(const_cast<wchar_t*>(wszShaderTag)), iShaderTagLen * sizeof(wchar_t));
	wstring wstrShaderTag = wszShaderTag;

	int32 iNumTextures = 0;
	_ifFile.read(reinterpret_cast<char*>(&iNumTextures), sizeof(iNumTextures));
	for (int32 i = 0; i < iNumTextures; ++i)
	{
		int32 iTextureUsageLen = 0;
		wchar_t wszTextureUsage[MAX_PATH] = {};
		_ifFile.read(reinterpret_cast<char*>(&iTextureUsageLen), sizeof(iTextureUsageLen));
		_ifFile.read(reinterpret_cast<char*>(const_cast<wchar_t*>(wszTextureUsage)), iTextureUsageLen * sizeof(wchar_t));

		int32 iTexturePathLen = 0;
		wchar_t wszTexturePath[MAX_PATH] = {};
		_ifFile.read(reinterpret_cast<char*>(&iTexturePathLen), sizeof(iTexturePathLen));
		_ifFile.read(reinterpret_cast<char*>(const_cast<wchar_t*>(wszTexturePath)), iTexturePathLen * sizeof(wchar_t));

		wstring wstrTexturePath = wszTexturePath;

		if (wstrTexturePath.find(L"../../Resource/") == wstring::npos)
		{
			wstrTexturePath = wstrTexturePath.substr(0, wstrTexturePath.find(L"."));
			wstrTexturePath = L"../../Resource/Models/Materials/" + wstrTexturePath + L".png";
		}

		ComPtr<ID3D11ShaderResourceView> cpTexture = CTextureHandler::GetInstance()->FindShaderResourceView(wstrTexturePath, ETextureType::MODEL);

		if (!cpTexture)
		{
			ENSURE(CTextureHandler::GetInstance()->AddShaderResourceView(wstrTexturePath, wstrTexturePath, ETextureType::MODEL));
			cpTexture = CTextureHandler::GetInstance()->FindShaderResourceView(wstrTexturePath, ETextureType::MODEL);
		}

		m_umapSRVs.insert({ wszTextureUsage, cpTexture });
	}

	int32 iNumScalars = 0;
	_ifFile.read(reinterpret_cast<char*>(&iNumScalars), sizeof(iNumScalars));
	for (int32 i = 0; i < iNumScalars; ++i)
	{
		int32 iScalarUsageLen = 0;
		wchar_t wszScalarUsage[MAX_PATH] = {};
		_ifFile.read(reinterpret_cast<char*>(&iScalarUsageLen), sizeof(iScalarUsageLen));
		_ifFile.read(reinterpret_cast<char*>(const_cast<wchar_t*>(wszScalarUsage)), iScalarUsageLen * sizeof(wchar_t));

		float fScalar = 0;
		_ifFile.read(reinterpret_cast<char*>(&fScalar), sizeof(fScalar));

		m_umapFloats.insert({ wszScalarUsage,fScalar });
	}

	if (_wstrModelFilePath.find(L"Models/Dat/NonAnim/SkyDome") != wstring::npos)
	{
		m_eShaderType = EShaderType::SKY_NONANIM;
	}
	else if (_wstrModelFilePath.find(L"Models/Dat/NonAnim/Architecture") != wstring::npos)
	{
		m_eShaderType = EShaderType::ARCHITECTURE;
	}
	else if (m_wstrMaterialName == L"MI_Pal_Fire_Kitsunebi")
	{
		m_eShaderType = EShaderType::MONSTER_KITSUNEBI_FIRE;
	}
	else
	{
		// 몬스터 표정
		if (wstrShaderTag == L"MSM_ClearCoat" && (m_wstrMaterialName.find(L"_Eye") != wstring::npos || m_wstrMaterialName.find(L"_Mouth") != wstring::npos))
		{
			m_eShaderType = EShaderType::MONSTERFACE_ANIM;
		}
		// 기본
		else if (wstrShaderTag == L"" || wstrShaderTag == L"Default" || wstrShaderTag == L"MSM_ClearCoat" || wstrShaderTag == L"M_PalLit_Env" || wstrShaderTag == L"M_PalLit_Foliage" || wstrShaderTag == L"MSM_Subsurface" || wstrShaderTag == L"MSM_Unlit")
		{
			switch (_eMeshType)
			{
				// 고유 모델 메쉬
			case Engine::EMeshManageType::UNIQUE:
				m_eShaderType = _eModelType == EModelType::ANIM ? EShaderType::DEFAULT_ANIM : EShaderType::DEFAULT_NONANIM;
				break;

				// 인스턴싱 모델 메쉬
			case Engine::EMeshManageType::INSTANCE:
				m_eShaderType = EShaderType::DEFAULT_INSTANCE;
				break;
			}
		}
		// 플레이어 눈
		else if (wstrShaderTag == L"Player_Eye")
		{
			m_eShaderType = EShaderType::PLAYEREYE_ANIM;
		}
		// 환경 - 윗면 혼합
		else if (wstrShaderTag == L"M_PalLit_Top")
		{
			m_eShaderType = _eMeshType == EMeshManageType::UNIQUE ? EShaderType::PALLIT_TOP_NONANIM : EShaderType::PALLIT_TOP_INSTANCE;
		}
		// 지형
		else if (wstrShaderTag == L"Landscape")
		{
			m_eShaderType = EShaderType::LANDSCAPE_NONANIM;
		}
		// 외부 에셋 - 나무 같은 외부 에셋을 변환 한 것
		else if (wstrShaderTag == L"Extern")
		{
			m_eShaderType = _eMeshType == EMeshManageType::UNIQUE ? EShaderType::EXTERN_NONANIM : EShaderType::EXTERN_INSTANCE;
		}
	}

	// 각종 제어값 추가
	switch (m_eShaderType)
	{
	case Engine::EShaderType::PLAYEREYE_ANIM:
		/*홍채 종류*/					m_umapInts.insert({ L"Cornea Index"						, 0 });
		/*광채 종류*/					m_umapInts.insert({ L"Highlight Index"					, 0 });
		/*동공 종류*/					m_umapInts.insert({ L"Pupil Index"						, 0 });
		/*홍채 대체 색상 사용 여부*/	m_umapInts.insert({ L"Cornea Use Alternative Color"      , false });
		/*광채 대체 색상 사용 여부*/	m_umapInts.insert({ L"Highlight Use Alternative Color"       , false });
		/*동공 대체 색상 사용 여부*/	m_umapInts.insert({ L"Pupil Use Alternative Color"	, false });
		/*홍채 대체 색상*/				m_umapVector3s.insert({ L"Cornea Alternative Color"			, Vector3(0,0.7f,0.9f) });
		/*광채 대체 색상*/				m_umapVector3s.insert({ L"Highlight Alternative Color"		, Vector3(1,1,1) });
		/*동공 대체 색상*/				m_umapVector3s.insert({ L"Pupil Alternative Color"			, Vector3(1,1,1) });
		/*홍채 발광 강도*/				m_umapFloats.insert({ L"Cornea Emision"					, 0.75f });
		/*광채 발광 강도*/				m_umapFloats.insert({ L"Highlight Emission"				, 0.f });
		/*동공 발광 강도*/				m_umapFloats.insert({ L"Pupli Emission"					, 0.f });
		break;
	case Engine::EShaderType::MONSTERFACE_ANIM:
		if (m_wstrMaterialName.find(L"Eye") != wstring::npos)
		{
			/*표정 종류*/	m_umapInts.insert({ L"MaxIndex X"	, 2 });
			/*현재 표정*/	m_umapInts.insert({ L"MaxIndex Y"	, 4 });
		}
		else if (m_wstrMaterialName.find(L"Mouth") != wstring::npos)
		{
			/*표정 종류*/	m_umapInts.insert({ L"MaxIndex X"	, 2 });
			/*현재 표정*/	m_umapInts.insert({ L"MaxIndex Y"	, 2 });
		}

		/*현재 표정 종류*/	m_umapInts.insert({ L"Index X"		, 0 });
		/*현재 표정 종류*/	m_umapInts.insert({ L"Index Y"		, 0 });

		break;
	case Engine::EShaderType::DEFAULT_ANIM:
	case Engine::EShaderType::DEFAULT_NONANIM:
	case Engine::EShaderType::DEFAULT_INSTANCE:
		/* 대체 색상 사용 여부	*/m_umapInts.insert({ L"Use Alternative Color", false });
		/* 대체 색상			*/m_umapVector3s.insert({ L"Alternative Color", Vector3(1.f ,1.f ,1.f) });
		break;
	case Engine::EShaderType::PALLIT_TOP_NONANIM:
	case Engine::EShaderType::PALLIT_TOP_INSTANCE:
		break;
	case Engine::EShaderType::EXTERN_NONANIM:
	case Engine::EShaderType::EXTERN_INSTANCE:
		break;
	case Engine::EShaderType::LANDSCAPE_NONANIM:
		/*보로노이 노이즈 샘플링 셀 크기 X*/					m_umapFloats.insert({ L"CellSize X"		, 20.f });
		/*보로노이 노이즈 샘플링 셀 크기 Y*/					m_umapFloats.insert({ L"CellSize Y"		, 20.f });
		/*보로노이 노이즈 샘플링 셀 당 기본 텍스처 반복 수 X*/	m_umapFloats.insert({ L"NumRepeat X"	, 1.f });
		/*보로노이 노이즈 샘플링 셀 당 기본 텍스처 반복 수 X*/	m_umapFloats.insert({ L"NumRepeat Y"	, 1.f });
		break;
	case Engine::EShaderType::SKY_NONANIM:
		break;
	case Engine::EShaderType::ARCHITECTURE:
		/*상태*/				m_umapInts.insert({ L"State",	0 });					// EArchitextureState 참고
		/*투명도*/				m_umapFloats.insert({ L"Alpha",	0.5f });				// EArchitextureState::BLUEPRINT 일 때의 투명도	( 0 ~ 1 )
		/*경계상자 Y축 길이*/	m_umapFloats.insert({ L"ExtendY",	1.f });				// EArchitextureState::BUILDING 일 때의 Y 길이	( 0 ~ N )
		/*현재 작업 진행률*/	m_umapFloats.insert({ L"Progress",	0.f });				// EArchitextureState::BUILDING 일 때의 진행도	( 0 ~ 1 )
		/*중심점*/			m_umapVector3s.insert({ L"Center", Vector3(0,0,0) });	// EArchitextureState::BUILDING 일 때의 Y값 계측용 중심점
		break;
	}

	return S_OK;
}

void Engine::CMaterial::Release()
{
	for (auto& pair : m_umapSRVs)
	{
		pair.second = nullptr;
	}
	m_umapSRVs.clear();
}

ComPtr<ID3D11ShaderResourceView> Engine::CMaterial::GetSRV(const wstring& _wstrUsage)
{
	if (m_umapSRVs.contains(_wstrUsage)) return m_umapSRVs[_wstrUsage];
	return nullptr;
}

int32 Engine::CMaterial::GetInt(const wstring& _wstrUsage)
{
	if (m_umapInts.contains(_wstrUsage)) return m_umapInts[_wstrUsage];
	return 0;
}

float Engine::CMaterial::GetFloat(const wstring& _wstrUsage)
{
	if (m_umapFloats.contains(_wstrUsage))return m_umapFloats[_wstrUsage];
	return 0.0f;
}

Vector2 Engine::CMaterial::GetVector2(const wstring& _wstrUsage)
{
	if (m_umapVector2s.contains(_wstrUsage))return m_umapVector2s[_wstrUsage];
	return Vector2();
}

Vector3 Engine::CMaterial::GetVector3(const wstring& _wstrUsage)
{
	if (m_umapVector3s.contains(_wstrUsage))return m_umapVector3s[_wstrUsage];
	return Vector3();
}

Vector4 Engine::CMaterial::GetVector4(const wstring& _wstrUsage)
{
	if (m_umapVector4s.contains(_wstrUsage))return m_umapVector4s[_wstrUsage];
	return Vector4();
}

Matrix Engine::CMaterial::GetMatrix(const wstring& _wstrUsage)
{
	if (m_umapmatrices.contains(_wstrUsage))return m_umapmatrices[_wstrUsage];
	return Matrix();
}

void Engine::CMaterial::SetSRV(wstring _wstrUsage, ComPtr<ID3D11ShaderResourceView> _cpSRV)
{
	if (m_umapSRVs.contains(_wstrUsage))
	{
		m_umapSRVs[_wstrUsage] = nullptr;
	}
	m_umapSRVs[_wstrUsage] = _cpSRV;
	m_bUpdated = true;
}