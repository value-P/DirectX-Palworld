#pragma once
#include "EffectBase.h"

namespace Engine
{
	class CInstanceEffectMeshComponent;
}

namespace ToolAnimation
{
	struct FInstanceEffectMeshObjectDesc : FEffectBaseDesc
	{
		std::wstring m_wstrModelFilePath = L"";
		std::wstring m_wstrJsonFileName = L"";

		Matrix m_matPivot = XMMatrixIdentity();
	};

	class CInstanceEffectMeshObject final : public CEffectBase
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CInstanceEffectMeshObject() = default;
		virtual ~CInstanceEffectMeshObject() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CInstanceEffectMeshObject> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, FEffectMeshInfo _tEffectMeshInfo, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo);
		static std::shared_ptr<CInstanceEffectMeshObject> CreateFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);
		HRESULT InitializeFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);
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

		/********************
			Getter / Setter
		********************/
	public:
		std::shared_ptr<CInstanceEffectMeshComponent> GetInstanceEffectMeshCom() { CHECKF(m_spInstanceEffectMesh, L"EffectMesh Component is nullptr : EffectObject"); return m_spInstanceEffectMesh; }

		uint32 GetCurKeyFrameIndex();
		std::vector<FEffectMeshKeyFrame>& GetKeyFrames();

		void SetObjectEffectMeshInfo(const FEffectMeshInfo& _tEffectMeshInfo) { m_tEffectMeshInfo = _tEffectMeshInfo; }
		void SetObjectInstanceEffectMeshInfo(const FInstanceEffectMeshInfo& _tInstanceEffectMeshInfo) { m_tInstanceEffectMeshInfo = _tInstanceEffectMeshInfo; }

		void SetJsonFileName(const std::wstring& _wstrJsonFileName) { m_wstrJsonFileName = _wstrJsonFileName; }
		void SetModelFilePath(const std::wstring& _wstrModelFilePath) { m_wstrModelFilePath = _wstrModelFilePath; }

		void SetPivotMatrix(Matrix _matPivot) { m_matPivot = _matPivot; }


	private:
		/* �ν��Ͻ� �޽� ������Ʈ */
		std::shared_ptr<CInstanceEffectMeshComponent> m_spInstanceEffectMesh = nullptr;

		/* ����Ʈ �޽� Info */
		FEffectMeshInfo m_tEffectMeshInfo;

		/* �ν��Ͻ� �޽� Info */
		FInstanceEffectMeshInfo m_tInstanceEffectMeshInfo;

	private:
		/* Json ���� */
		std::wstring m_wstrJsonFileName = L"";

		/* �� ���� */
		std::wstring m_wstrModelFilePath = L"";

		/* �ǹ� ��Ʈ���� */
		Matrix m_matPivot;
	};
}
