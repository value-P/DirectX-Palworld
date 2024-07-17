#pragma once
#include "Component.h"

namespace Engine
{
	class CSoundHandler;

	struct ENGINE_DLL FSoundComponentDesc : public CComponent::FComponentDesc
	{
		std::wstring m_wstrSoundFileName = L"";

		bool m_bLoop = false;

		float m_fCurVolume = 1.0f;

		float m_fMinVolume = 0.0f;

		float m_fMaxVolume = 1.0f;
	};

    class ENGINE_DLL CSoundComponent: public CComponent
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CSoundComponent() = default;

		virtual ~CSoundComponent() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CSoundComponent> Create(const std::shared_ptr<FSoundComponentDesc>& _spSoundComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render() override;

		virtual void Release() override;

		virtual HRESULT BindVariable() override;

	/********************
		Data Members
	********************/
	private:
		std::wstring m_wstrSoundFileName = L"";

		bool m_bLoop = false;

		float m_fCurVolume = 1.0f;

		float m_fMinVolume = 0.0f;

		float m_fMaxVolume = 1.0f;

	private:
		std::weak_ptr<CSoundHandler> m_wpSoundHandler;
    };
}
