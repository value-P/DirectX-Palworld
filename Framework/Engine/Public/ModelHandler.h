#pragma once

/* 03.24 박동연 - 이펙트 툴에서의 이펙트 메시들 리스트를 띄우기 위해 추가 */

namespace Engine
{
	class CThreadManager;

	class ENGINE_DLL CModelHandler final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CModelHandler)

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
		HRESULT LoadEffectMeshPaths();

		/* 24-04-07 이동준 추가 */
		HRESULT LoadGridLandscapeMeshFromFile();
		HRESULT LoadGridEnvMeshFromFile();
		HRESULT LoadGridEnvCollisionFromFile();
		HRESULT LoadPlaceableTreeMeshFromFile();
		HRESULT LoadPlaceableRockMeshFromFile();
		HRESULT LoadPlaceableWoodMeshFromFile();
		HRESULT LoadPlaceableStoneMeshFromFile();
		HRESULT LoadPlaceablePalMeshFromFile();
		HRESULT LoadPlaceableArchitectureMeshFromFile();
		HRESULT LoadPlaceableLightMeshFromFile();
		HRESULT LoadPlaceableCameraMeshFromFile();

		/* 24-04-09 이동준 추가 */
		HRESULT LoadMapMeshFromFile(EMapMeshType _eMapMeshType);


	/********************
		Getter/Setter
	********************/
	public:
		const std::vector<std::wstring>& GetEffectMeshPaths() { return m_vecEffectMeshPaths; }

		const std::wstring& GetEffectMeshPath() { return m_wstrEffectMeshPath; }

	public:
		// 24-04-07 이동준 추가
		const std::vector<std::wstring>& GetGridLandscapeMeshFilePaths() { return m_vecGridLandscapeMeshFilePaths; }					// Landscape
		const std::vector<std::wstring>& GetGridEnvMeshFilePaths() { return m_vecGridEnvMeshFilePaths; }								// EnvMesh
		const std::vector<std::wstring>& GetGridEnvCollisionFilePaths() { return m_vecGridEnvCollisionFilePaths; }						// EnvCollision
		const std::vector<std::wstring>& GetPlaceableTreeMeshFilePaths() { return m_vecPlaceableTreeMeshFilePaths; }					// Tree
		const std::vector<std::wstring>& GetPlaceableRockMeshFilePaths() { return m_vecPlaceableRockMeshFilePaths; }					// Rock
		const std::vector<std::wstring>& GetPlaceableWoodMeshFilePaths() { return m_vecPlaceableWoodMeshFilePaths; }					// Wood
		const std::vector<std::wstring>& GetPlaceableStoneMeshFilePaths() { return m_vecPlaceableStoneMeshFilePaths; }					// Stone
		const std::vector<std::wstring>& GetPlaceablePalMeshFilePaths() { return m_vecPlaceablePalMeshFilePaths; }						// Pal
		const std::vector<std::wstring>& GetPlaceableArchitectureMeshFilePaths() { return m_vecPlaceableArchitectureMeshFilePaths; }	// Architecture
		const std::vector<std::wstring>& GetPlaceableLightMeshFilePaths() { return m_vecPlaceableLightMeshFilePaths; }					// Light
		const std::vector<std::wstring>& GetPlaceableCameraMeshFilePaths() { return m_vecPlaceableCameraMeshFilePaths; }				// Camera


		/********************
			Data Members
		********************/
	private:
		std::vector<std::wstring> m_vecEffectMeshPaths;

		std::wstring m_wstrEffectMeshPath = L"../../Resource/Models/Dat/NonAnim/EffectMeshs/";

		/* 24-04-07 이동준 추가 */ /* Grid */
	private:
		std::vector<std::wstring> m_vecGridLandscapeMeshFilePaths; // Landscape
		std::wstring m_wstrGridLandscapeMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Grid/Landscape/";
		std::vector<std::wstring> m_vecGridEnvMeshFilePaths; // Env
		std::wstring m_wstrGridEnvMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Grid/Env/EnvMesh/";
		std::vector<std::wstring> m_vecGridEnvCollisionFilePaths;
		std::wstring m_wstrGridEnvCollisionFilePath = L"../../Resource/Models/Dat/MapMesh/Grid/Env/EnvCollision/";

		/* 24-04-07 이동준 추가 */ // Placeable
	private:
		std::vector<std::wstring> m_vecPlaceableTreeMeshFilePaths; // Tree
		std::wstring m_wstrPlaceableTreeMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Tree/";
		std::vector<std::wstring> m_vecPlaceableRockMeshFilePaths; // Rock
		std::wstring m_wstrPlaceableRockMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Rock/";
		std::vector<std::wstring> m_vecPlaceableWoodMeshFilePaths; // Wood
		std::wstring m_wstrPlaceableWoodMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Wood/";
		std::vector<std::wstring> m_vecPlaceableStoneMeshFilePaths; // Stone
		std::wstring m_wstrPlaceableStoneMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Stone/";
		std::vector<std::wstring> m_vecPlaceablePalMeshFilePaths; // Pal
		std::wstring m_wstrPlaceablePalMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Pal/";
		std::vector<std::wstring> m_vecPlaceableArchitectureMeshFilePaths; // Architecture
		std::wstring m_wstrPlaceableArchitectureMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Architecture/";
		std::vector<std::wstring> m_vecPlaceableLightMeshFilePaths; // Light // TODO: 경로 바꾸기
		std::wstring m_wstrPlaceableLightMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Architecture/";
		std::vector<std::wstring> m_vecPlaceableCameraMeshFilePaths; // Camera // TODO: 경로 바꾸기
		std::wstring m_wstrPlaceableCameraMeshFilePath = L"../../Resource/Models/Dat/MapMesh/Placeable/Architecture/";


	private:
		std::weak_ptr<CThreadManager> m_wpThreadManager;
	};
}
