#include "pch.h"
#include "ModelHandler.h"

#include "EngineModule.h"
#include "ThreadManager.h"

IMPLEMENT_SINGLETON(CModelHandler)

HRESULT Engine::CModelHandler::Initialize()
{
	m_wpThreadManager = CEngineModule::GetInstance()->GetThreadManager();

	m_vecEffectMeshPaths.reserve(100);

	return S_OK;
}

void Engine::CModelHandler::Release()
{
	m_vecEffectMeshPaths.clear();
	
	/* 24-04-07 이동준 추가 */
	m_vecGridLandscapeMeshFilePaths.clear();
	m_vecGridEnvMeshFilePaths.clear();
	m_vecGridEnvCollisionFilePaths.clear();
	m_vecPlaceableTreeMeshFilePaths.clear();
	m_vecPlaceableRockMeshFilePaths.clear();
	m_vecPlaceableWoodMeshFilePaths.clear();
	m_vecPlaceableStoneMeshFilePaths.clear();

	m_spInstance = nullptr;
}

HRESULT Engine::CModelHandler::LoadEffectMeshPaths()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrEffectMeshPath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecEffectMeshPaths.push_back(fsEntry.path());
	}

	return S_OK;
}

HRESULT Engine::CModelHandler::LoadGridLandscapeMeshFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrGridLandscapeMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecGridLandscapeMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadGridEnvMeshFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrGridEnvMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecGridEnvMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadGridEnvCollisionFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrGridEnvCollisionFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecGridEnvCollisionFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadPlaceableTreeMeshFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrPlaceableTreeMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecPlaceableTreeMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadPlaceableRockMeshFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrPlaceableRockMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecPlaceableRockMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadPlaceableWoodMeshFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrPlaceableWoodMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecPlaceableWoodMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadPlaceableStoneMeshFromFile()
{
	return S_OK;

	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrPlaceableStoneMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecPlaceableStoneMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadPlaceablePalMeshFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrPlaceablePalMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecPlaceablePalMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadPlaceableArchitectureMeshFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrPlaceableArchitectureMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecPlaceableArchitectureMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadPlaceableLightMeshFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrPlaceableLightMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecPlaceableLightMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadPlaceableCameraMeshFromFile()
{
	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(m_wstrPlaceableCameraMeshFilePath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		m_vecPlaceableCameraMeshFilePaths.push_back(fsEntry.path().generic_wstring());
	}

	// TODO: 스레드 매니저에서 작업 수를 감소시켜야 합니다.
	return S_OK;
}

HRESULT Engine::CModelHandler::LoadMapMeshFromFile(EMapMeshType _eMapMeshType)
{
	wstring wstrMapMeshPath = L"";

	switch (_eMapMeshType)
	{
	case EMapMeshType::GRID:			wstrMapMeshPath = m_wstrGridLandscapeMeshFilePath;												break;
	case EMapMeshType::ENVMESH:			wstrMapMeshPath = m_wstrGridEnvMeshFilePath;													break;
	case EMapMeshType::ENVCOLLISION:	wstrMapMeshPath = m_wstrGridEnvCollisionFilePath;												break;
	case EMapMeshType::TREE:			wstrMapMeshPath = m_wstrPlaceableTreeMeshFilePath;												break;
	case EMapMeshType::ROCK:			wstrMapMeshPath = m_wstrPlaceableRockMeshFilePath;												break;
	case EMapMeshType::WOOD:			wstrMapMeshPath = m_wstrPlaceableWoodMeshFilePath;												break;
	case EMapMeshType::STONE:			wstrMapMeshPath = m_wstrPlaceableStoneMeshFilePath;												break;
	case EMapMeshType::PAL:				wstrMapMeshPath = m_wstrPlaceablePalMeshFilePath;												break;
	case EMapMeshType::ARCHITECTURE:	wstrMapMeshPath = m_wstrPlaceableArchitectureMeshFilePath;										break;
	case EMapMeshType::LIGHT:			wstrMapMeshPath = m_wstrPlaceableLightMeshFilePath;												break;
	case EMapMeshType::CAMERA:			wstrMapMeshPath = m_wstrPlaceableCameraMeshFilePath;											break;
	case EMapMeshType::ENUM_END:		CHECKF(false, L"Failed to LoadMapMeshFromFile, MapMesh type is ENUM_END. : CModelHandler");		break;
	default:							CHECKF(false, L"Failed to LoadMapMeshFromFile, MapMesh type is not defined at CModelHandler.")	break;
	}

	for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(wstrMapMeshPath))
	{
		// 파일이 아닌 경우
		if (!filesystem::is_regular_file(fsEntry)) { continue; }

		switch (_eMapMeshType)
		{
		case EMapMeshType::GRID:			m_vecGridLandscapeMeshFilePaths.push_back(fsEntry.path().generic_wstring());					break;
		case EMapMeshType::ENVMESH:			m_vecGridEnvMeshFilePaths.push_back(fsEntry.path().generic_wstring());							break;
		case EMapMeshType::ENVCOLLISION:	m_vecGridEnvCollisionFilePaths.push_back(fsEntry.path().generic_wstring());						break;
		case EMapMeshType::TREE:			m_vecPlaceableTreeMeshFilePaths.push_back(fsEntry.path().generic_wstring());					break;
		case EMapMeshType::ROCK:			m_vecPlaceableRockMeshFilePaths.push_back(fsEntry.path().generic_wstring());					break;
		case EMapMeshType::WOOD:			m_vecPlaceableWoodMeshFilePaths.push_back(fsEntry.path().generic_wstring());					break;
		case EMapMeshType::STONE:			m_vecPlaceableStoneMeshFilePaths.push_back(fsEntry.path().generic_wstring());					break;
		case EMapMeshType::PAL:				m_vecPlaceablePalMeshFilePaths.push_back(fsEntry.path().generic_wstring());						break;
		case EMapMeshType::ARCHITECTURE:	m_vecPlaceableArchitectureMeshFilePaths.push_back(fsEntry.path().generic_wstring());			break;
		case EMapMeshType::LIGHT:			m_vecPlaceableLightMeshFilePaths.push_back(fsEntry.path().generic_wstring());					break;
		case EMapMeshType::CAMERA:			m_vecPlaceableCameraMeshFilePaths.push_back(fsEntry.path().generic_wstring());					break;
		case EMapMeshType::ENUM_END:		CHECKF(false, L"Failed to LoadMapMeshFromFile, MapMesh type is ENUM_END. : CModelHandler");		break;
		default:							CHECKF(false, L"Failed to LoadMapMeshFromFile, MapMesh type is not defined at CModelHandler.")	break;
		}
	}

	return S_OK;
}