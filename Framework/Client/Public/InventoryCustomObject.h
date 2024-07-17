#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
	class CCameraManager;
}

namespace Client
{
	struct FInventoryCustomObjectDesc : public CGameObject::FGameObjectDesc
	{
	};

    class CInventoryCustomObject : public CGameObject
    {
    /********************
		Ctor/Dtor
	********************/
	public:
		explicit CInventoryCustomObject() = default;

		virtual ~CInventoryCustomObject() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CInventoryCustomObject> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

		virtual void SetActive(bool _bActive) override;

	private:
		void InitMesh(const std::shared_ptr<FInventoryCustomObjectDesc>& _spGameObjectDesc);

		void InitDefaultBodyMesh(const std::shared_ptr<FInventoryCustomObjectDesc>& _spGameObjectDesc);

		void InitWeaponMesh();

		void InitDisable();

		void LoadMeshComponent(const wstring& _wstrFilePath, unordered_map<EItemType, shared_ptr<CMeshComponent>>& _umapMeshContainer, EItemType _eItemType, const wstring& _wstrAttachBoneName = L"");

		void SetCustomColor();

	/********************
		Getter/Setter
	********************/
	public:
		void ChangeHeadEquipWithID(EItemType _eItemType);
		void ChangeOutfitWithID(EItemType _eItemType);
		void ChangeWeaponWithID(EItemType _eItemType);

	/********************
		Data Members
	********************/
	protected:
		weak_ptr<CCameraManager> m_wpCameraManager;

		shared_ptr<CMeshComponent> m_spAnimRefMesh = nullptr; // ref bone¿ë ¸Þ½¬
		shared_ptr<CMeshComponent> m_spHeadMesh = nullptr;
		shared_ptr<CMeshComponent> m_spHairMesh = nullptr;

		unordered_map<EItemType, shared_ptr<CMeshComponent>> m_umapHeadEquipMesh;
		unordered_map<EItemType, shared_ptr<CMeshComponent>> m_umapOutfitMesh;
		unordered_map<EItemType, shared_ptr<CMeshComponent>> m_umapWeaponMesh;

		EItemType m_eCurHeadEquipID = EItemType::NONE;
		EItemType m_eCurOutfitID = EItemType::Outfit_OldCloth001;
		EItemType m_eCurWeaponID = EItemType::NONE;

		Vector3 m_vCamOffset = Vector3(0.017f, 0.04f, 0.2f);
    };
}


