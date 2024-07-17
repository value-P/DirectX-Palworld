#pragma once
#include "Item.h"

namespace Engine
{
	class CSphereComponent;
}

namespace Client
{
	class CToDoSystem;
	class CMonster;

	/********************
		Descriptor
	********************/
	struct FMineralDesc : public FItemDesc
	{
		EMineralType m_eCurMineralType = EMineralType::ENUM_END;

		bool m_bPrefab = true;
	};

	class CMineral : public CItem
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CMineral() = default;

		virtual ~CMineral() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CMineral> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

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
		const EMineralType GetMineralType() const { return m_eMineralType; }

		void SetMineralType(const EMineralType _eMineralType) { m_eMineralType = _eMineralType; }

	private:
		bool IsCommandRegistered() const { return m_iCurCommandID != -1; }

		bool IsCommandAssigned() const;

	/********************
		Methods
	********************/
	private:
		void InitMeshComponent();

		void InitColliderComponent();

	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

		virtual bool CollectItem() override;

		virtual void SetPhysxTransformMatrix(const Matrix& _matTransform);

	public:
		int32 RegisterCommandToSystem(EToDoType _eToDoType, EMonsterTaskType _eMonsterTaskType);

		HRESULT UnRegisterCommandFromSystem(EToDoType _eToDoType, int32 _iCurCommandID);

		HRESULT AddWorkingSlavePal(const std::shared_ptr<CMonster>& _spMonster);

		HRESULT RemoveWorkingSlavePal();

	public:
		virtual void SetPosition(const Vector3& _vPosition) override;

		virtual const std::optional<Vector3> GetPosition() const override;

		void SetGravity(bool _bGravity);

	/********************
		Data Members
	********************/
	private:
		// �۾� ���� �� �ĺ���
		int32 m_iCurCommandID = -1;

		// ���ϰ� �ִ� ��
		std::weak_ptr<CMonster> m_wpWorkingSlavePal;

	private:
		EMineralType m_eMineralType = EMineralType::ENUM_END;

		// ������ ����; �⺻���� ���������� �����մϴ�.
		bool m_bPrefab = true;

	private:
		std::shared_ptr<CSphereComponent> m_spBodySphereComponent = nullptr;
	};
}
