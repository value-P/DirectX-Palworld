#pragma once
#include "RectComponent.h"

namespace Engine
{
	class ENGINE_DLL CButtonComponent : public CRectComponent
	{
	/********************
		Description
	********************/
	public:
		struct ENGINE_DLL FButtonComponentDesc : public FRectComponentDesc
		{
			std::wstring m_wstrTextureFileNameButtonStatic = L"";
			std::wstring m_wstrTextureFileNameButtonHover = L"";
			std::wstring m_wstrTextureFileNameButtonClick = L"";
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CButtonComponent() = default;

		virtual ~CButtonComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CButtonComponent> Create(const std::shared_ptr<FButtonComponentDesc>& _spButtonComponentDesc);

		static std::shared_ptr<CButtonComponent> CreateFromJson(const std::shared_ptr<FButtonComponentDesc>& _spButtonComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex);

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
		bool IsHoverd();
		
		bool IsClicked();

		void AddButtonClickEvent(std::function<void(void)> _fnEvent)
		{
			m_fnButtonClick = [_fnEvent]() { _fnEvent(); };
		}

	/********************
		Getter/Setter
	********************/
	public:
		std::wstring GetStaticTextureFileName() const { return m_wstrTextureFileNameButtonStatic; }
		
		std::wstring GetHoverTextureFileName() const { return m_wstrTextureFileNameButtonHover; }
		
		std::wstring GetClickTextureFileName() const { return m_wstrTextureFileNameButtonClick; }

	/********************
		Data Members
	********************/
	private:
		//텍스쳐를 넣어줍니다.
		ComPtr<ID3D11ShaderResourceView> m_cpSRVButtonStatic = nullptr;

		ComPtr<ID3D11ShaderResourceView> m_cpSRVButtonHover = nullptr;

		ComPtr<ID3D11ShaderResourceView> m_cpSRVButtonClick = nullptr;

		std::wstring m_wstrTextureFileNameButtonStatic = L"";

		std::wstring m_wstrTextureFileNameButtonHover = L"";

		std::wstring m_wstrTextureFileNameButtonClick = L"";

		bool m_bHover = false;

		bool m_bClick = false;

	private:
		std::function<void(void)> m_fnButtonClick = nullptr;
	};
}
