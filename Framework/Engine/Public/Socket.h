#pragma once
#include "GameObject.h"

namespace Engine
{
	class ENGINE_DLL CSocket : public CGameObject
	{
	/********************
		Description
	********************/
	public:
		struct FSocketDesc : public FGameObjectDesc
		{
			string m_strBoneName = "";
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CSocket() = default;

		virtual ~CSocket() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CSocket> Create(const std::shared_ptr<FSocketDesc>& _spSocketDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		void SetBoneName(const std::string& _strBoneName) { m_strBoneName = _strBoneName; }

	/********************
		Data Members
	********************/
	protected:
		// 본 행렬
		Matrix m_matSocket = Matrix::Identity;

		// 본 이름
		std::string m_strBoneName = "";
	};
}
