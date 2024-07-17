
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
		/* 오브젝트 풀에서 꺼낼 때 불릴 함수 */
		void RecycleFromPool();

		/* 비활성화 시 실행 */
		void NoneActivate();

		/* 종료 시 실행 */
		void EndLifeTime();

		/* 루프 시 실행 */
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
		/* 이펙트 메시 컴포넌트 */
		std::shared_ptr<CEffectMeshComponent> m_spEffectMesh;

		/* 이펙트 메시 Info */
		FEffectMeshInfo m_tEffectMeshInfo;

		/* 매터리얼 */// 노멀, 매탈릭 등을 위해 추가 - 준영이의 요청 -
		std::shared_ptr<CMaterial> m_spMaterial;

	private:
		/* Json 파일 */
		std::wstring m_wstrJsonFileName = L"";

		/* 모델 파일 */
		std::wstring m_wstrModelFilePath = L"";

		/* 피벗 매트릭스 */
		Matrix m_matPivot;
	};
}
