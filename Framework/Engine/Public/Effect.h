#pragma once
#include "GameObject.h"

/* �����Դϴ�. Ȥ���� ���� �ؼ� ���ܳ����ϴ�. */

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
		/* ����Ʈ �޽� */
		std::weak_ptr<CEffectMeshComponent> m_wpEffectMesh;

	protected:
		/* ���� Ÿ�� */
		ERenderType m_eRenderType = ERenderType::ENUM_END;

		/* Json ���� */
		std::wstring m_wstrJsonFileName = L"";

		/* �� ���� ��� */
		std::wstring m_wstrModelFilePath = L"";

		/* �ǹ� ��Ʈ���� */
		Matrix	m_matPivot = Matrix::Identity;
	};
}
