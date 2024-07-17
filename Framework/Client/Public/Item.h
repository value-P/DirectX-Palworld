#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
	class CSphereComponent;
	class CBone;
}

namespace Client
{
	class CMonster;
	class CToDoSystem;
	class CInventorySystem;
	
	/********************
		Descriptor
	********************/
	struct FItemDesc : public CGameObject::FGameObjectDesc
	{
		EItemType m_eCurItemType = EItemType::ENUM_END;

		EItemKindType m_eCurItemKindType = EItemKindType::ENUM_END;

		int32 m_iItemCount = 0;
	};

	class CItem : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CItem() = default;

		virtual ~CItem() = default;
		
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
		int32 GetNumItems() const { return m_iItemCount; }

		void SetNumItems(int32 _iNumItems) { m_iItemCount = _iNumItems; }

		EItemType GetItemType() const { return m_eCurItemType; }

		void SetItemType(const EItemType _eItemType) { m_eCurItemType = _eItemType; }

	public:
		const EItemKindType GetItemKindType() const { return m_eItemKindType; }

		void SetItemKindType(const EItemKindType _eItemKindType) { m_eItemKindType = _eItemKindType; }
		
	/********************
		Methods
	********************/
	public:
		// ���� ���� �������� ����մϴ�.
		virtual void TransportItem(const std::shared_ptr<CBone>& _spBone, const Matrix& _matWorld);

		virtual void SetPhysxTransformMatrix(const Matrix& _matTransform);

		// �÷��̾ �������� �����մϴ�.
		virtual bool CollectItem();
		
	public:
		int32 RegisterCommandToSystem(EToDoType _eToDoType, EMonsterTaskType _eMonsterTaskType);

		HRESULT UnRegisterCommandFromSystem(EToDoType _eToDoType, int32 _iCurCommandID = -1);

		HRESULT AddWorkingSlavePal(const std::shared_ptr<CMonster>& _spMonster);

		HRESULT RemoveWorkingSlavePal(int32 _iGameObjectID);

	/********************
		Data Members
	********************/
	protected:
		// ������ Ÿ��
		EItemType m_eCurItemType = EItemType::ENUM_END;

		EItemKindType m_eItemKindType = EItemKindType::ENUM_END;

		// ������ ����
		int32 m_iItemCount = 0;

	protected:
		// �۾� ���� �� �ĺ���
		int32 m_iCurCommandID = -1;

		// ���ϰ� �ִ� �ӵ�
		std::unordered_map<int32, std::weak_ptr<CMonster>> m_umapWorkingSlavePals;

		std::weak_ptr<CToDoSystem> m_wpToDoSystem;

		std::weak_ptr<CInventorySystem> m_wpInventorySystem;

	protected:
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
	};
}
