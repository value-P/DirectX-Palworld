#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
	class CBoxComponent;
}

namespace Client
{
	struct FBossRoomDesc : public CGameObject::FGameObjectDesc
	{
		std::wstring m_wstrBossRoomFilePath = L"";
	};

	class CBossRoom final : public CGameObject
	{
	public:
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CBossRoom() = default;

		virtual ~CBossRoom() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CBossRoom> Create(const std::shared_ptr<FBossRoomDesc>& _spBossRoomDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
	};
}
