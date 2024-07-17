#pragma once
#include "Component.h"

namespace Engine
{
	class CModel;
	class CEffectShader;
	class CPickManager;
	class CMeshComponent;
	class CBone;
	class CShaderHandler;

	struct ENGINE_DLL FMeshComponentDesc : public CComponent::FComponentDesc
	{
		std::wstring m_wstrModelFilePath = L"";

		std::wstring m_wstrShaderName = L"";

		std::wstring m_wstrUpperBoneName = L"";

		std::wstring m_wstrAnimationGroupJsonPath = L"";

		Matrix m_matPivot;

		std::shared_ptr<CBone> m_spPivotBone = nullptr;

		std::shared_ptr<CMeshComponent> m_spParentMeshComponent = nullptr;

		EMeshManageType m_eMeshType = EMeshManageType::UNIQUE;
	};

	class ENGINE_DLL CMeshComponent : public CComponent
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CMeshComponent() = default;

		virtual ~CMeshComponent() = default;

		/********************
			Events
		********************/
	public:
		static shared_ptr<CMeshComponent> Create(const std::shared_ptr<FComponentDesc>& _spComponentDesc);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc);

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual HRESULT BindVariable() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

		/********************
			Getter/Setter
		********************/
	public:
		EModelType GetModelType() const;

		shared_ptr<CModel> GetModel() const { return m_spModel; }

		int32 GetMeshCount(EShaderType _eShaderType) const;

		void SetRotationExceptBone(const wstring& _wstrBoneName);

		int32 GetUpperCurrentLoop();
		int32 GetDownCurrentLoop();

		const std::wstring& GetModelFilePath() { return m_wstrModelFilePath; }

		shared_ptr<CBone> GetBonePtr(const wstring& _wstrBoneName);
		shared_ptr<CBone> GetBonePtr(int32 _iBoneIndex);

		void SetUseAnimation(bool _bUseAnimation, bool _bUseIdentityMatrices = false);

		FModelAnimData GetModelAnimationData();

		void SetTrackPosition(float _fTrackPosition);

		void ShakeUpperBody(float _fShakePower, float _fShakeTime);

		/********************
			Methods
		********************/
	public:
		void ChangeAnimation(int32 _iNextAnim, bool _bPlayReverse = false, float _fLoopStartRate = 0.f);
		void ChangeAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse = false, float _fLoopStartRate = 0.f);
		void ChangeAnimation(const wstring& _wstrAnimName);
		void ChangeDifferentAnimation(int32 _iNextAnim, bool _bPlayReverse = false, float _fLoopStartRate = 0.f);

		// 상체 변경
		void ChangeUpperAnimation(int32 _iNextAnim, bool _bPlayReverse = false, float _fLoopStartRate = 0.f);
		void ChangeUpperAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse = false, float _fLoopStartRate = 0.f);

		// 하체 변경
		void ChangeDownAnimation(int32 _iNextAnim, bool _bPlayReverse = false, float _fLoopStartRate = 0.f);
		void ChangeDownAnimation(int32 _iNextAnim, float _fLerpTime, bool _bPlayReverse = false, float _fLoopStartRate = 0.f);

		// 특정본에 로테이션 먹이기(라디안)
		void SetRotationAtBone(const wstring& _wstrBoneName, Vector3 _vRotation);

		/********************
			Data Members
		********************/
	protected:
		ComPtr<ID3D11Device> m_cpDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;
		weak_ptr<CShaderHandler> m_wpShaderHandler;

		unordered_map<EShaderType, shared_ptr<CEffectShader>> m_umapShaders;
		unordered_map<EShaderType, ComPtr<ID3DX11EffectVariable>> m_umapVariables_MeshInfo;
		unordered_map<EShaderType, ComPtr<ID3DX11EffectShaderResourceVariable>> m_umapVariables_Textures;

		std::weak_ptr<CModel> m_wpParentModel;

		std::shared_ptr<CModel> m_spModel = nullptr;

		EMeshManageType m_eMeshManageType = EMeshManageType::ENUM_END;

		std::weak_ptr<CBone> m_wpPivotBone;

		std::wstring m_wstrModelFilePath = L"";

		Matrix m_matPivot;

		bool m_bPlayingAnimation = true;
	};
}