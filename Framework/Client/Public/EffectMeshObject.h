
#pragma once
#include "EffectBase.h"

namespace Engine
{
	class CEffectMeshComponent;
	class CMaterial;
}

namespace Client
{
	struct FEffectMeshObjectDesc : public FEffectBaseDesc
	{
		std::wstring m_wstrModelFilePath = L"";
		std::wstring m_wstrJsonFileName = L"";

		Matrix m_matPivot = XMMatrixIdentity();
	};

	class CEffectMeshObject : public CEffectBase
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CEffectMeshObject() = default;
		virtual ~CEffectMeshObject() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CEffectMeshObject> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, FEffectMeshInfo _tEffectMeshInfo);
		static std::shared_ptr<CEffectMeshObject> CreateFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);
		virtual HRESULT InitializeFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);
		virtual HRESULT BeginPlay() override;
		virtual int32 PreTick(float _fDeltaSeconds) override;
		virtual int32 Tick(float _fDeltaSeconds) override;
		virtual int32 PostTick(float _fDeltaSeconds) override;
		virtual HRESULT EndPlay() override;
		virtual void Release() override;
		
	/********************
		Methods
	********************/
	public:
		/* ������Ʈ Ǯ���� ���� �� �Ҹ� �Լ� */
		void RecycleFromPool();

		/* ��Ȱ��ȭ �� ���� */
		void NoneActivate();

		/* ���� �� ���� */
		void EndLifeTime();

		/* ���� �� ���� */
		void Loop();


	/********************
		Getter / Setter
	********************/
	public:
		std::shared_ptr<CEffectMeshComponent> GetEffectMeshCom() { CHECKF(m_spEffectMesh, L"EffectMesh Component is nullptr : EffectObject"); return m_spEffectMesh; }

		uint32 GetCurKeyFrameIndex();
		std::vector<FEffectMeshKeyFrame>& GetKeyFrames();

		void SetObjectEffectMeshInfo(const FEffectMeshInfo& _tEffectMeshInfo){ m_tEffectMeshInfo = _tEffectMeshInfo; }

		void SetJsonFileName(const std::wstring& _wstrJsonFileName) { m_wstrJsonFileName = _wstrJsonFileName; }
		void SetModelFilePath(const std::wstring& _wstrModelFilePath) { m_wstrModelFilePath = _wstrModelFilePath; }

		void SetPivotMatrix(Matrix _matPivot) { m_matPivot = _matPivot; }

	/********************
		Data Members
	********************/
	private:
		/* ����Ʈ �޽� ������Ʈ */
		std::shared_ptr<CEffectMeshComponent> m_spEffectMesh;

		/* ����Ʈ �޽� Info */
		FEffectMeshInfo m_tEffectMeshInfo;

		/* ���͸��� */// ���, ��Ż�� ���� ���� �߰� - �ؿ����� ��û -
		std::shared_ptr<CMaterial> m_spMaterial;

	private:
		/* Json ���� */
		std::wstring m_wstrJsonFileName = L"";

		/* �� ���� */
		std::wstring m_wstrModelFilePath = L"";

		/* �ǹ� ��Ʈ���� */
		Matrix m_matPivot;
	};
}
