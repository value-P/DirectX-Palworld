#pragma once

namespace Client
{
	class CGameManager;
	class CCreature;
	class CPlayer;
	class CMonster;

	/********************
		Descriptor
	********************/
	struct FCombatSystemDesc final
	{
		std::shared_ptr<CGameManager> m_spGameManager = nullptr;
	};

	class CCombatSystem final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CCombatSystem() = default;

		virtual ~CCombatSystem() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CCombatSystem> Create(const std::shared_ptr<FCombatSystemDesc>& _spCombatSystemDesc);

		HRESULT Initialize(const std::shared_ptr<FCombatSystemDesc>& _spCombatSystemDesc);
		
	public:
		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);

		int32 PostTick(float _fDeltaSeconds);

		HRESULT EndPlay();

		void Release();
		
	/********************
		Methods
	********************/
	public:
		HRESULT AddCombatPal(const std::shared_ptr<CMonster>& _spMonster);

		HRESULT RemoveCombatPal(const std::shared_ptr<CMonster>& _spMonster);

		HRESULT RemoveCombatPal(int32 _iGameObjectID);

	public:
		HRESULT CommandToPartnerPal();

		HRESULT CommandToWildPal(const std::shared_ptr<CMonster>& _spMonster);

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CGameManager> m_wpGameManager;

		// 플레이어와 싸우고 있는 몬스터들
		std::unordered_map<int32, std::weak_ptr<CMonster>> m_umapMonsters;

		// 플레이어
		std::shared_ptr<CPlayer> m_spPlayer = nullptr;
	};
}
