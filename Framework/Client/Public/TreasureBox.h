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
	struct FTreasureBoxDesc : public CGameObject::FGameObjectDesc
	{

	};

	class CTreasureBox : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTreasureBox() = default;

		virtual ~CTreasureBox() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTreasureBox> Create(const std::shared_ptr<FTreasureBoxDesc>& _spTreasureBoxDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release();

	/********************
		Overrides
	********************/
	public:
		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	/********************
		Methods
	********************/
	public:
		// 보석상자 랜덤으로 뭔가 나오게 하기
		void ReturnRandomReward();

	private:
		void InitMeshComponent();

		void InitColliderComponent();

	/********************
		Getter/Setter
	********************/
	public:

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;

		std::shared_ptr<CSphereComponent> m_spSphereComponent = nullptr;
	};
}