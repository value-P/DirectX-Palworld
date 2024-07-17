#pragma once
#include "RectComponent.h"

namespace Engine
{
	class ENGINE_DLL CSliderComponent : public CRectComponent
	{
		/********************
			Description
		********************/
	public:
		struct ENGINE_DLL FSliderComponentDesc : public FRectComponentDesc
		{
			std::wstring m_wstrTextureSliderButtonFileName = L"";
			weak_ptr<CRectComponent> m_wpBackGroundSliderComponent;
		};

		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CSliderComponent() = default;

		virtual ~CSliderComponent() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CSliderComponent> Create(const std::shared_ptr<FSliderComponentDesc>& _spSliderComponentDesc);

		static std::shared_ptr<CSliderComponent> CreateFromJson(const std::shared_ptr<FSliderComponentDesc>& _spSliderComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

		virtual HRESULT BindVariable() override;

		/********************
			Methods
		********************/
	public:
		virtual bool IsDragged() override;

		void Normalize();

		/********************
			Getter/Setter
		********************/
	public:
		void SetNormalize(float _fNormalize) { m_fNormalize = _fNormalize; }

		float GetNormalize() const { return m_fNormalize; }

		std::wstring GetTextureFileName() const { return m_wstrTextureSliderButtonFileName; }

		std::wstring GetBackGroundComponentName() const;

		void SetBackGroundSliderComponent(weak_ptr<CRectComponent> _wpBackGroundComponent) { m_wpBackGroundSliderComponent = _wpBackGroundComponent; }

		/********************
			Data Members
		********************/
	private:
		std::wstring m_wstrTextureSliderButtonFileName = L"";

		weak_ptr<CRectComponent> m_wpBackGroundSliderComponent;

		float m_fMin = 0.0f;

		float m_fMax = 0.0f;

		// BackGroundSlider와 SliderButton의 거리에 따른 비율을 저장합니다.
		float m_fNormalize = 0.0f;
	};
}
