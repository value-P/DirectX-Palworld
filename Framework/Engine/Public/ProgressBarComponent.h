#pragma once
#include "RectComponent.h"

namespace Engine
{
	class ENGINE_DLL CProgressBarComponent : public CRectComponent
	{
		/********************
			Description
		********************/
	public:
		struct ENGINE_DLL FProgressBarComponentDesc : public FRectComponentDesc
		{
			std::wstring m_wstrTextureFileName = L"";
		};

		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CProgressBarComponent() = default;

		virtual ~CProgressBarComponent() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CProgressBarComponent> Create(const std::shared_ptr<FProgressBarComponentDesc>& _spProgressComponentDesc);

		static std::shared_ptr<CProgressBarComponent> CreateFromJson(const std::shared_ptr<FProgressBarComponentDesc>& _spProgressComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex);

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
			Getter/Setter
		********************/
	public:
		std::wstring GetTextureFileName() const { return m_wstrTextureFileName; }

		void SetOtherColor(Vector4 _vOtherColor) { m_tUiInfoProgressBar.vOtherColor = _vOtherColor; }

		const Vector4 GetOtherColor() const { return m_tUiInfoProgressBar.vOtherColor; }

		void SetOtherRatio(float _fOtherRatio) { m_tUiInfoProgressBar.fOtherRatio = _fOtherRatio; }

		float GetOtherRatio() const { return m_tUiInfoProgressBar.fOtherRatio; }

		/********************
			Data Members
		********************/
	private:
		// Bind
		FUiInfo_ProgressBar m_tUiInfoProgressBar = {};

		// Bind Variables
		ComPtr<ID3DX11EffectVariable> m_cpVariable_ProgressBar = nullptr;

		std::wstring m_wstrTextureFileName = L"";
	};
}
