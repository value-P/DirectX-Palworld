#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
	class CPanel;
	class CButtonComponent;
	class CTextComponent;
	class CCamera;
	class CImageComponent;
}

namespace Client
{
	struct FCustomizeObjectDesc : public CGameObject::FGameObjectDesc
	{
		shared_ptr<CPanel> m_spCustomizeBackgroundUI = nullptr;
		shared_ptr<CCamera> m_spCamera = nullptr;
	};

    class CCustomizeObject : public CGameObject
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CCustomizeObject() = default;

		virtual ~CCustomizeObject() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CCustomizeObject> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	private:
		void InitMesh();
		void InitUI();

		void LoadMeshComponent(const wstring& _wstrFilePath, vector<pair<shared_ptr<CMeshComponent>, wstring>>& _vecMeshContainer);
		void LoadMeshComponent(const wstring& _wstrFilePath, vector<pair<shared_ptr<CMeshComponent>, pair<EItemType, wstring>>>& _vecMeshContainer, EItemType _eItemType, const wstring& _wstrAttachBoneName = L"");

	private:
		void ComputeCameraPos();

	/********************
		Getter/Setter
	********************/
	public:
		void ChangeHeadToNext();
		void ChangeHairToNext();
		void ChangeHeadEquipToNext();
		void ChangeOutfitToNext();
		void ChangeCorneaToNext();
		void ChangeHighlightToNext();

		void ChangeHeadToPrev();
		void ChangeHairToPrev();
		void ChangeHeadEquipToPrev();
		void ChangeOutfitToPrev();
		void ChangeCorneaToPrev();
		void ChangeHighlightToPrev();

		void CustomizeColor(const Vector3& _vRGB);

		FCustomObjectData GetCustomObjectData();
				
	/********************
		Getter/Setter
	********************/
	private:
		void UpdateButtonEvent();
		void UpdateCustomizeUIText();
		void UpdateColor();

	/********************
		Data Members
	********************/
	protected:
		shared_ptr<CMeshComponent> m_spAnimRefMesh = nullptr; // ref bone용 메쉬

		vector<pair<shared_ptr<CMeshComponent>,wstring>> m_vecHeadMesh; // 얼굴 21종
		vector<pair<shared_ptr<CMeshComponent>,wstring>> m_vecHairMesh; // 헤어스타일 30종

		vector<pair<shared_ptr<CMeshComponent>, pair<EItemType,wstring>>> m_vecHeadEquipMesh;
		vector<pair<shared_ptr<CMeshComponent>, pair<EItemType, wstring>>> m_vecOutfitMesh;

		int32 m_iCurHeadIndex = 0;
		int32 m_iCurHairIndex = 0;
		int32 m_iCurHeadEquipIndex = -1; // -1일 경우 아무것도 착용 X
		int32 m_iCurOutfitIndex = 0;

		int32 m_iCorneaIndex = 0;
		int32 m_iHighlightIndex = 2;
		
		Vector3 m_vCustomColor = Vector3(0.5f, 0.5f, 0.5f);

	private:
		std::weak_ptr<CPanel> m_wpCustomizeBackgroundUI;

		std::weak_ptr<CButtonComponent> m_wpHairPrevSelectButton;
		std::weak_ptr<CButtonComponent> m_wpHairNextSelectButton;
		std::weak_ptr<CTextComponent> m_wpHairNameText;

		std::weak_ptr<CButtonComponent> m_wpHeadPrevSelectButton;
		std::weak_ptr<CButtonComponent> m_wpHeadNextSelectButton;
		std::weak_ptr<CTextComponent> m_wpHeadNameText;

		std::weak_ptr<CButtonComponent> m_wpOutfitPrevSelectButton;
		std::weak_ptr<CButtonComponent> m_wpOutfitNextSelectButton;
		std::weak_ptr<CTextComponent> m_wpOutfitNameText;

		std::weak_ptr<CButtonComponent> m_wpHeadEquipPrevSelectButton;
		std::weak_ptr<CButtonComponent> m_wpHeadEquipNextSelectButton;
		std::weak_ptr<CTextComponent> m_wpHeadEquipNameText;

		std::weak_ptr<CImageComponent> m_wpColorR;
		std::weak_ptr<CImageComponent> m_wpColorG;
		std::weak_ptr<CImageComponent> m_wpColorB;

	private:
		weak_ptr<CCamera> m_wpCustomizeLevelCam;

		Vector3 m_vFaceCamPos = Vector3(0.f, 1.5f, 1.f);
		Vector3 m_vBodyCamPos = Vector3(0.f, 1.f, 2.5f);

		Vector3 m_vDstCamPos = Vector3(0.f, 1.f, 2.5f);
	};
}