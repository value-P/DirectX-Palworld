#pragma once

namespace Engine
{
	class CLevel;
	class CLayer;

	/*
	* @detail
	* 월드 클래스는 게임에서 플레이할 모든 레벨을 갖고 있는 싱글턴 클래스입니다.
	*/
	class ENGINE_DLL CWorld final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		DECLARE_SINGLETON(CWorld)
		
	/********************
		Events
	********************/
	public:
		// 폐기 예정
		HRESULT Initialize
		(
			const std::shared_ptr<CLevel>& _spPersistentLevel,
			const std::shared_ptr<CLevel>& _spCurLevel,
			const std::shared_ptr<CLevel>& _spLoadLevel
		);

		HRESULT Initialize
		(
			const std::shared_ptr<CLevel>& _spPersistentLevel,
			const std::shared_ptr<CLevel>& _spLoadLevel
		);

		HRESULT BeginPlay();

		int32 Tick(float _fDeltaSeconds);

		void Clear();

		void Release();
		
	/********************
		Getter/Setter
	********************/
	public:
		shared_ptr<CLevel> GetPersistentLevel() const { return m_spPersistentLevel; }

		// 폐기 예정
		shared_ptr<CLevel> GetCurLevel() const { return m_spFrontLevel; }

		shared_ptr<CLevel> GetPostFrontLevel() const { return m_spPostFrontLevel; }

		void SetPostFrontLevel(const std::shared_ptr<CLevel>& _spPostFrontLevel) { m_spPostFrontLevel = _spPostFrontLevel; }

		shared_ptr<CLevel> GetFrontLevel() const { return m_spFrontLevel; }

		shared_ptr<CLevel> GetBackLevel() const { return m_spBackLevel; }

	/********************
		Methods
	********************/
	public:
		HRESULT LoadLevel(int32 _iPostLevelType);

		HRESULT OpenLevel
		(
			const std::shared_ptr<CLevel>& _spPostFrontLevel,
			const std::shared_ptr<CLevel>& _spPostBackLevel
		);

	private:
		HRESULT ClearLevel();

	/********************
		Data Members
	********************/
	public:
		// 게임 오브젝트 식별자 카운터
		uint32 m_iGameObjectID = 0;

	private:
		// 로드 레벨
		std::shared_ptr<CLevel> m_spLoadLevel = nullptr;

		// 영구 레벨
		std::shared_ptr<CLevel> m_spPersistentLevel = nullptr;

		// 백 레벨(영구 레벨 혹은 널 포인터)
		std::shared_ptr<CLevel> m_spBackLevel = nullptr;

		// 프론트 레벨(로드 레벨 혹은 클라이언트 레벨)
		std::shared_ptr<CLevel> m_spFrontLevel = nullptr;

		// 다음 레벨(클라이언트 레벨)
		std::shared_ptr<CLevel> m_spPostFrontLevel = nullptr;

		// 현재 레벨(폐기 예정)
		// std::shared_ptr<CLevel> m_spCurLevel = nullptr;
	};
}
