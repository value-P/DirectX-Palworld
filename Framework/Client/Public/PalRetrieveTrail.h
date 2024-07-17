#pragma once
#include "EffectGroup.h"
#include "TrailComponent.h"

namespace Engine
{
	class CTrailComponent;
	class CBone;
}

namespace Client
{
	class CEffectManager;
	class CPlayer;

	struct FPalRetrieveTrailDesc : public CGameObject::FGameObjectDesc
	{
		
	};

	class CPalRetrieveTrail final : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPalRetrieveTrail() = default;
		virtual ~CPalRetrieveTrail() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPalRetrieveTrail> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	public:
		HRESULT InitializeTrailComponent();
		
	/********************
		Methods
	********************/
	public:
		void TickTrail(float _fTrailSize, float _fDeltaSeconds);
		void OnTrail(const Vector3& _vStartPos, const Vector3 _vEndPos, float _fTrailSize);
		void OffTrail();

	/********************
		Getter / Setter
	********************/
	public:
		

	/********************
		Data Members
	********************/
	private:
		// ����Ʈ �Ŵ���
		std::weak_ptr<CEffectManager> m_wpEffectManager;

		// Ʈ����
		std::shared_ptr<CTrailComponent> m_spTrailCom = nullptr;

	private:
		// Ʈ���� Ȱ��ȭ
		bool m_bActiveTrail = true;

		// Ʈ���� ũ��
		float m_fTrailSize = 0.1f;

		// Ʈ���� Ȱ��ȭ �ð�
		float m_fTrailActiveAcc = 0.f;

		// ���� ����
		Vector3 m_vStartPos = Vector3(0.f);

		// �� ����
		Vector3 m_vEndPos = Vector3(0.f);

		// �÷��̾�
		weak_ptr<CPlayer> m_wpPlayer;

		// �÷��̾� �� ��
		weak_ptr<CBone> m_wpPlayerHandBone;
	};
}