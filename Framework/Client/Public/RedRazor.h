#pragma once
#include "EffectGroup.h"
#include "TrailComponent.h"

namespace Engine
{
	class CModel;
}

namespace Client
{
	class CEffectManager;
	class CCreature;
	class CPlayer;
	class CElecPanda;

	struct FRedRazorDesc : public FEffectGroupDesc
	{
		
	};

	class CRedRazor final : public CEffectGroup
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CRedRazor() = default;
		virtual ~CRedRazor() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CRedRazor> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);
		static std::shared_ptr<CRedRazor> CreateFromJson(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc, const std::wstring& _wstrJsonFileName);

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
		void SetPlayer(std::shared_ptr<CPlayer> _spPlayer) { m_spPlayer = _spPlayer; }
		void SetElecPanda(std::shared_ptr<CElecPanda> _spElecPanda) { m_spElecPanda = _spElecPanda; }

	/********************
		Data Members
	********************/
	private:
		// 이펙트 매니저
		std::weak_ptr<CEffectManager> m_wpEffectManager;

	private:
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;

		std::shared_ptr<CElecPanda> m_spElecPanda = nullptr;
	};
}