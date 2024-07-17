#pragma once
#include "GameObject.h"

/* 깡통입니다. 혹여나 쓸까 해서 남겨놓습니다. */

namespace Engine
{
	class CEffectMeshComponent;

	class ENGINE_DLL CEffect : public CGameObject
	{
	/********************
		Description
	********************/
	public:
		struct ENGINE_DLL FEffectDesc : public FGameObjectDesc
		{

		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CEffect() = default;

		virtual ~CEffect() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CEffect> Create(const std::shared_ptr<FEffectDesc>& _spEffectDesc);
		
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
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
		virtual void RecycleFromPool();

	/********************
		Getter / Setter
	********************/
	public:


	/********************
		Data Members
	********************/
	protected:
		/* 이펙트 메시 */
		std::weak_ptr<CEffectMeshComponent> m_wpEffectMesh;

	protected:
		/* 렌더 타입 */
		ERenderType m_eRenderType = ERenderType::ENUM_END;

		/* Json 파일 */
		std::wstring m_wstrJsonFileName = L"";

		/* 모델 파일 경로 */
		std::wstring m_wstrModelFilePath = L"";

		/* 피벗 매트릭스 */
		Matrix	m_matPivot = Matrix::Identity;
	};
}
