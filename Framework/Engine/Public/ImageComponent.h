#pragma once
#include "RectComponent.h"

namespace Engine
{
	class ENGINE_DLL CImageComponent : public CRectComponent
	{
	/********************
		Description
	********************/
	public:
		struct ENGINE_DLL FImageComponentDesc : public FRectComponentDesc
		{
			std::wstring m_wstrTextureFileName = L"";

			EImageType m_eImageType = EImageType::DEFAULT;
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CImageComponent() = default;

		virtual ~CImageComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CImageComponent> Create(const std::shared_ptr<FImageComponentDesc>& _spImageComponentDesc);

		static std::shared_ptr<CImageComponent> CreateFromJson(const std::shared_ptr<FImageComponentDesc>& _spImageComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex);

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
		wstring GetTextureName() const { return m_wstrTextureFileName; }

		void SetImageType(EImageType _eImageType) { m_eImageType = _eImageType; }

		EImageType GetImageType() const { return m_eImageType; }

		void SetTextureFileName(const std::wstring _wstrTextureFileName);

	/********************
		Data Members
	********************/
	private:
		std::wstring m_wstrTextureFileName = L"";

		EImageType m_eImageType = EImageType::DEFAULT;
	};
}
