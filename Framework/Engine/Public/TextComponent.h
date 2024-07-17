#pragma once
#include "RectComponent.h"

namespace Engine
{
	class CFontHandler;
	class CFont;

	class ENGINE_DLL CTextComponent : public CRectComponent
	{
	/********************
		Description
	********************/
	public:
		struct ENGINE_DLL FTextComponentDesc : public FRectComponentDesc
		{
			std::wstring m_wstrFontFileName;

			std::wstring m_wstrText;

			Vector2 m_vOrigin = Vector2::Zero;

			int32 m_iOption = 0x0;

			ETextType m_eTextType = ETextType::LEFT;
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTextComponent() = default;

		virtual ~CTextComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTextComponent> Create(const std::shared_ptr<FTextComponentDesc>& _spTextComponentDesc);

		static std::shared_ptr<CTextComponent> CreateFromJson(const std::shared_ptr<FTextComponentDesc>& _spTextComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex);

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
		void SetText(const std::wstring& _wstrText) { m_wstrText = _wstrText; }

		std::wstring GetText() const { return m_wstrText; }

		void SetFontFileName(const std::wstring& _wstrFontFileName) { m_wstrFontFileName = _wstrFontFileName; }

		std::wstring GetFontFileName() const { return m_wstrFontFileName; }

		void SetFontDesc(FFontDesc& fFontDesc);

		void SetFontDesc();

		FFontDesc GetFontDesc();

		void SetScreenPosition(Vector2 _vScreenPosition);

		const Vector2 GetScreenPosition() const;

		void SetOption(int32 _iOption) { m_iOption = _iOption; }

		int32 GetOption() const { return m_iOption; }

		void SetTextType(ETextType _eTextType) { m_eTextType = _eTextType; }

		const ETextType GetTextType() const { return m_eTextType; }
		
	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CFontHandler> m_wpFontHandler;

		std::shared_ptr<CFont> m_spFont;
		
		std::wstring m_wstrFontFileName;

		std::wstring m_wstrText;

		Vector2 m_vOrigin = Vector2::Zero;
	
		int32 m_iOption = 0x0;

		ETextType m_eTextType = ETextType::LEFT;
	};
}

