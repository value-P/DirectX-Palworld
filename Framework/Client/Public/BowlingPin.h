#pragma once
#include "GameObject.h"

namespace Engine
{
	class CSoundManager;
	class CMeshComponent;
	class CBoxComponent;
}

namespace Client
{
	/********************
		Descriptor
	********************/
	struct FBowlingPinDesc : public CGameObject::FGameObjectDesc
	{
		
	};

	class CBowlingPin : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CBowlingPin() = default;

		virtual ~CBowlingPin() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CBowlingPin> Create(const std::shared_ptr<FBowlingPinDesc>& _spBowlingPinDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release();

		virtual void OnCollisionEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	/********************
		Methods
	********************/
	private:
		void InitMeshComponent();

		void InitColliderComponent();

	public:
		void RequestTransform(std::optional<Vector3> _vTranslate, std::optional<Vector3> _vRotate);

		void SetLockFlags(int32 _iAxisLockType);

		shared_ptr<CMeshComponent>& GetMeshComponent() { return m_spMeshComponent; }

	/********************
		Getter/Setter
	********************/
	public:
		void SetColl(bool _bColl) { m_bColl = _bColl; }
		bool IsColl() { return m_bColl; }
	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
		std::shared_ptr<CBoxComponent> m_spBoxComponent = nullptr;

	private:
		bool m_bColl = false;

	private:
		std::weak_ptr<CSoundManager> m_wpSoundManager;
	};
}