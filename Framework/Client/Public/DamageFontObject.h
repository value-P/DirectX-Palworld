#pragma once
#include "Panel.h"

namespace Engine
{
	class CTextComponent;
}

namespace Client
{
	class CUIManager;

	/********************
		Description
	********************/
	struct FDamageFontDesc : public CPanel::FPanelDesc
	{

	};

	class CDamageFontObject : public CPanel
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CDamageFontObject() = default;

		virtual ~CDamageFontObject() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CDamageFontObject> Create(const std::shared_ptr<FDamageFontDesc>& _spDamageFontDesc);

		static std::shared_ptr<CDamageFontObject> CreateFromJson(const std::wstring& _wstrJsonFileName);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
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
		void SetDamageText(int32 _iDamage);

		void ActiveDamageFont(Vector3 _vPosition);
		
		void LerpDamageFont(float _fDeltaSeconds);
		// 데미지 입는 순간에 호출해주면 됩니다. 
		void SetDamageFont(int32 _iDamage, Vector3 _vPosition);

		void SetDamageTextComponent(shared_ptr<CTextComponent> _spBase, shared_ptr<CTextComponent> _spDamage);

	private:
		/* 보간 타입에 따라 값 보간 */
		float EaseByType(EEasingType _eEasingType, float& _fStart, float& _fEnd, float _fRatio, float _fPower = 0.f);

		Vector3 EaseByType(EEasingType _eEasingType, Vector3& _vStart, Vector3& _vEnd, float _fRatio, float _fPower = 0.f);

		Vector4 EaseByType(EEasingType _eEasingType, Vector4& _vStart, Vector4& _vEnd, float _fRatio, float _fPower = 0.f);

	/********************
		Getter/Setter
	********************/

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CTextComponent> m_spBackText = nullptr;
		std::shared_ptr<CTextComponent> m_spFrontText = nullptr;

	private:
		std::weak_ptr<CUIManager> m_wpUIManager;

	private:
		bool m_bStart = false;

		float m_fLerpTime = 0.0f;

		Vector3 m_vStartPos;

		Vector3 m_vEndPos;

		EEasingType m_eEasingType = EEasingType::EASE_INOUT_CIRC;
	};
}