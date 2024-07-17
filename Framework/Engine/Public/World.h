#pragma once

namespace Engine
{
	class CLevel;
	class CLayer;

	/*
	* @detail
	* ���� Ŭ������ ���ӿ��� �÷����� ��� ������ ���� �ִ� �̱��� Ŭ�����Դϴ�.
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
		// ��� ����
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

		// ��� ����
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
		// ���� ������Ʈ �ĺ��� ī����
		uint32 m_iGameObjectID = 0;

	private:
		// �ε� ����
		std::shared_ptr<CLevel> m_spLoadLevel = nullptr;

		// ���� ����
		std::shared_ptr<CLevel> m_spPersistentLevel = nullptr;

		// �� ����(���� ���� Ȥ�� �� ������)
		std::shared_ptr<CLevel> m_spBackLevel = nullptr;

		// ����Ʈ ����(�ε� ���� Ȥ�� Ŭ���̾�Ʈ ����)
		std::shared_ptr<CLevel> m_spFrontLevel = nullptr;

		// ���� ����(Ŭ���̾�Ʈ ����)
		std::shared_ptr<CLevel> m_spPostFrontLevel = nullptr;

		// ���� ����(��� ����)
		// std::shared_ptr<CLevel> m_spCurLevel = nullptr;
	};
}
