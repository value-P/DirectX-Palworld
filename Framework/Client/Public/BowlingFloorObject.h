#pragma once
#include "GameObject.h"

namespace Engine
{
	class CBoxComponent;
}

namespace Client
{
	struct FBowlingFloorObject : public CGameObject::FGameObjectDesc
	{
	};

	class CBowlingFloorObject : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CBowlingFloorObject() = default;

		virtual ~CBowlingFloorObject() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CBowlingFloorObject> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT BeginPlay() override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CBoxComponent> m_spBoxComponent = nullptr;
	};
}