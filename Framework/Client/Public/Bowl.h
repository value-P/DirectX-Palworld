#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
	class CSphereComponent;
}

namespace Client
{
	/********************
		Descriptor
	********************/
	struct FBowlDesc : public CGameObject::FGameObjectDesc
	{
		
	};

	class CBowl : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CBowl() = default;

		virtual ~CBowl() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CBowl> Create(const std::shared_ptr<FBowlDesc>& _spBowlDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release();

	/********************
		Methods
	********************/
	public:
		// 콜라이더 중심으로 이동시키기 위해 오버라이드해서 사용합니다.
		virtual void LookAt(const Vector3& _vPosition) override;

		virtual void Turn(const Vector3& _vAxis, float _fAngularSpeed) override;

		virtual void MoveForward(float _fSpeed) override;

		virtual void MoveBackward(float _fSpeed) override;

		virtual void MoveRight(float _fSpeed) override;

		virtual void MoveLeft(float _fSpeed) override;

		virtual void MoveUp(float _fSpeed) override;

		virtual void MoveDown(float _fSpeed) override;

		virtual void SetPosition(const Vector3& _vPosition) override;

		void AddForce(const Vector3& _vDirection, float _fPower);

		void ChangeAnimation(int32 _iNextAnim);

		void RequestTransform(std::optional<Vector3> _vTranslate, std::optional<Vector3> _vRotate);

	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	private:
		void InitMeshComponent();

		void InitColliderComponent();

	/********************
		Getter/Setter
	********************/
	public:
		shared_ptr<CMeshComponent>& GetMeshComponent() { return m_spMeshComponent; }

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
		std::shared_ptr<CSphereComponent> m_spSphereComponent = nullptr;
	};
}