#pragma once

namespace Engine
{
	class CTextComponent;

	class CFont final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CFont() = default;

		virtual ~CFont() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CFont> Create(const std::wstring& _wstrFontFilePath);

		HRESULT Initialize(const std::wstring& _wstrFontFilePath);

		void Render();

		void Release();
		
	/********************
		Methods
	********************/
	public:
		void SetText(const std::wstring& _wstrText) { m_wstrText = _wstrText; }

		void SetScreenPosition(const Vector2& _vScreenPosition) { m_vScreenPosition = _vScreenPosition; }

		const Vector2 GetScreenPosition() const { return m_vScreenPosition; }

		void SetColor(const Vector4& _vColor) { m_vColor = _vColor; }

		void SetRadian(float _fRadian) { m_fRadian = _fRadian; }

		void SetOrigin(const Vector2& _vOrigin) { m_vOrigin = _vOrigin; }

		void SetScale(float _fScale) { m_fScale = _fScale; }

		void SetOption(int32 _iOption) { m_iOption = _iOption; }

		void SetTextComponent(std::shared_ptr<CTextComponent> _spTextComponent) { m_spTextComponent = _spTextComponent; }

		const std::shared_ptr<CTextComponent> GetTextComponent() const { return m_spTextComponent; }

	/********************
		Data Members
	********************/
	private:
		std::wstring m_wstrText = L"Sprite Font";

		Vector2 m_vScreenPosition = Vector2::Zero;

		Vector4 m_vColor = Vector4::One;

		float m_fRadian = 0.0f;

		Vector2 m_vOrigin = Vector2::Zero;

		float m_fScale = 1.0f;

		int32 m_iOption = 0x0;

	private:
		std::shared_ptr<SpriteBatch> m_spSpriteBatch = nullptr;

		std::shared_ptr<SpriteFont> m_spSpriteFont = nullptr;
		// 폰트가 소속된 컴포넌트입니다.
		std::shared_ptr<CTextComponent> m_spTextComponent = nullptr;

		ComPtr<ID3D11BlendState> m_cpBlendState = nullptr;
	};
}
