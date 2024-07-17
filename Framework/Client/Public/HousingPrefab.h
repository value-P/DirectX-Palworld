#pragma once
#include "ArchitecturePrefab.h"

namespace Engine
{
	class CMeshComponent;
	class CBoxComponent;
}

namespace Client
{
	class CHousing;

	struct FHousingPrefabDesc final : public FArchitecturePrefabDesc
	{
		EBuildType m_eBuildType = EBuildType::ENUM_END;

		std::unordered_map<EBuildType, EBuildType> m_umapNeededBuildTypes;
	};

	class CHousingPrefab : public CArchitecturePrefab
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHousingPrefab() = default;

		virtual ~CHousingPrefab() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHousingPrefab> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		/* ������ �ִ� �Ͽ�¡�� �ٽ��ϴ�. */
		void AttachToNearHousing();
		EBuildDir DecideBuildDir(const EBuildType& _eBuildType, const EBuildType& _eOpponentBuildType, float _fAngle);

		/* ���� �ش� �����տ� �ش��ϴ� ���๰�� ���� ������ �������� ������ üũ�մϴ�. */
		virtual bool IsBuildable() override;
		bool IsBuildable(EBuildType _eBuildType);
		void SetNoneAttached();

	/********************
		Getter/Setter
	********************/
	public:
		const EBuildType& GetBuildType() { return m_eBuildType; }
		const std::unordered_map<EBuildType, EBuildType>& GetNeededBuildTypes() { return m_umapNeededBuildTypes; }
		bool GetIsAttached() { return m_bAttached; }
		std::shared_ptr<CHousing> GetAttachedHousing()
		{ 
			if (!m_wpAttachedHousing.expired()) { return m_wpAttachedHousing.lock(); }
			else { return nullptr; }
		}
		const std::tuple<int32, int32, int32>& GetBuildIndex() { return m_tpBuildIndex; }
		int32 GetFloor() { return m_iFloor; }
		const EBuildDir& GetBuildDir() { return m_eBuildDir; }

	/********************
		Data Members
	********************/
	protected:
		/* �Ͽ�¡ ���� Ÿ�� */
		EBuildType m_eBuildType = EBuildType::ENUM_END;

		/* �Ͽ�¡�� ����Ƿ��� �ʿ��� ��� �Ͽ�¡�� ���� Ÿ�Ե� */
		std::unordered_map<EBuildType, EBuildType> m_umapNeededBuildTypes;

		/* Attach ���� */
		bool m_bAttached = false;

		/* Attach�� �� ��ϵ� �Ͽ�¡ ������ */
		std::weak_ptr<CHousing> m_wpAttachedHousing;

		/* �������� ����� ���� */
		EBuildDir m_eBuildDir = EBuildDir::ENUM_END;

		/* �������� ����� �ε��� */
		std::tuple<int32, int32, int32> m_tpBuildIndex = tuple(0, 0, 0);

		/* �� */
		int32 m_iFloor = 0;
	};
}
