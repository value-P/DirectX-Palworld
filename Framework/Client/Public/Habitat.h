#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
}

namespace Client
{
	class CTreeManager;
	class CAsyncManager;
	class CToDoSystem;
	class CGameManager;
	class CMonster;

	/********************
		Descriptor
	********************/
	struct FHabitatDesc : public CGameObject::FGameObjectDesc
	{
		EHabitatType m_eHabitatType = EHabitatType::ENUM_END;

		std::wstring m_wstrModelFilePath = L"";
	};

	class CHabitat : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHabitat() = default;

		virtual ~CHabitat() = default;
		
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		EHabitatType GetHabitatType() const { return m_eHabitatType; }

		const std::wstring& GetModelFilePath();

	/********************
		Methods
	********************/
	public:
		virtual HRESULT AddHabitatToScene();

		virtual HRESULT RemoveHabitatFromScene();

	public:
		int32 RegisterCommandToSystem(EToDoType _eToDoType, EMonsterTaskType _eMonsterTaskType);

		HRESULT UnRegisterCommandFromSystem(EToDoType _eToDoType, int32 _iCurCommandID = -1);

		HRESULT ChangeCommandState(EToDoType _eToDoType, EToDoCommandState _eToDoCommandState);

		HRESULT AddWorkingSlavePal(const std::shared_ptr<CMonster>& _spMonster);

		HRESULT RemoveWorkingSlavePal(int32 _iGameObjectID);

	/********************
		Data Members
	********************/
	protected:
		std::weak_ptr<CTreeManager> m_wpTreeManager;

		std::weak_ptr<CAsyncManager> m_wpAsyncManager;

		std::weak_ptr<CToDoSystem> m_wpToDoSystem;

		std::weak_ptr<CGameManager> m_wpGameManager;

	protected:
		EHabitatType m_eHabitatType = EHabitatType::ENUM_END;

	protected:
		std::unordered_map<std::wstring, std::shared_ptr<CMeshComponent>> m_umapMeshComponents;

		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

	protected:
		// 작업 중인 일 식별자
		int32 m_iCurCommandID = -1;

		// 일하고 있는 팰들
		std::unordered_map<int32, std::weak_ptr<CMonster>> m_umapWorkingSlavePals;
	};
}
