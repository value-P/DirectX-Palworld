#pragma once
#include "Architecture.h"

namespace Client
{

	class CHousing;
	class CHousingLump;

	struct FHousingDesc : public FArchitectureDesc
	{
		EBuildType m_eBuildType = EBuildType::ENUM_END;

		EBuildDir m_eBuildDir = EBuildDir::ENUM_END;

		std::unordered_map<EBuildType, EBuildType> m_umapNeededBuildTypes;

		std::shared_ptr<CHousingLump> m_spHousingLump = nullptr;

		std::tuple<int32, int32, int32> m_tpBuildIndex = tuple(0, 0, 0);

		int32 m_iFloor = 0;
	};

	class CHousing : public CArchitecture
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHousing() = default;

		virtual ~CHousing() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHousing> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;
		
		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	public:
		virtual void RecycleFromPool() override;

		virtual void ReturnToPool() override;

	/********************
		Methods
	********************/
	public:
		/* ���� ���⿡ �Ͽ�¡ �߰� */
		HRESULT AddCombinedHousing(EBuildDir _eBuildDir, std::shared_ptr<CHousing> _spHousing);

		/* ���� ���⿡ ����� �Ͽ�¡ �˻� */
		std::shared_ptr<CHousing> GetCombinedHousing(EBuildDir _eBuildDir);

		/* �ı� */
		virtual void DestroyArchitecture() override;

		// �ӽ�
		HRESULT SaveHousing(rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer);

	private:
		virtual HRESULT InitializeMeshComponent() override;
		virtual HRESULT InitializeColliderComponent() override;
		
	/********************
		Getter/Setter
	********************/
	public:
		/* ���� Ÿ�� */
		const EBuildType& GetBuildType() const { return m_eBuildType; }

		/* ���� ���� */
		const EBuildDir& GetBuildDir() const { return m_eBuildDir; }

		/* �� �Ͽ�¡�� ����Ƿ��� �ʿ��� ��� �Ͽ�¡�� ���� Ÿ�Ե� */
		const std::unordered_map<EBuildType, EBuildType>& GetNeededBuildTypes() { return m_umapNeededBuildTypes; }

		/* �Ͽ�¡ ��� */
		std::shared_ptr<CHousingLump> GetHousingLump() { CHECKF(!m_wpHousingLump.expired(), L"m_wpHousingLump is expired : CHousing"); return m_wpHousingLump.lock(); }

		/* �θ� �Ͽ�¡ */
		void SetParentHousing(EBuildDir _eBuildDir, std::shared_ptr<CHousing> _spHousing) { m_prParentHousing.first = _eBuildDir; m_prParentHousing.second = _spHousing; }
		std::optional<pair<EBuildDir, std::shared_ptr<CHousing>>> GetParentHousing()
		{
			if (!m_prParentHousing.second) { return m_prParentHousing; }
			else { return nullopt; }
		}

		/* �Ͽ�¡�� ����Ǿ� ������ �ڽ� �Ͽ�¡�� */
		std::vector<std::shared_ptr<CHousing>>& GetCombinedHousings() { return m_vecCombinedHousings; }

		/* ���� �ε��� */
		const tuple<int32, int32, int32>& GetBuildIndex() { return m_tpBuildIndex; }

		/* �� */
		int32 GetFloor() { return m_iFloor; }

		const wstring& GetHousingModelFilePath() { return m_wstrModelFilePath; };

	/********************
		Data Members
	********************/
	protected:
		/* ���� Ÿ�� */
		EBuildType m_eBuildType = EBuildType::ENUM_END;

		/* ���� ���� */
		EBuildDir m_eBuildDir = EBuildDir::ENUM_END;

		/* �� �Ͽ�¡�� ����Ƿ��� �ʿ��� ��� �Ͽ�¡�� ���� Ÿ�Ե� */
		std::unordered_map<EBuildType, EBuildType> m_umapNeededBuildTypes;

		/* �Ͽ�¡ ��� */
		std::weak_ptr<CHousingLump> m_wpHousingLump;

		/* �θ� �Ͽ�¡�� ����� ���� */
		std::pair<EBuildDir, std::shared_ptr<CHousing>> m_prParentHousing;

		/* �� �Ͽ�¡�� ����Ǿ� ������ �Ͽ�¡�� */
		std::vector<std::shared_ptr<CHousing>> m_vecCombinedHousings;

		/* ������ ���(0, 0, 0) ���� 3���� ���� ���� �ε��� ��ġ */
		std::tuple<int32, int32, int32> m_tpBuildIndex = tuple(0, 0, 0);

		/* �� */
		int32 m_iFloor = 0;
	};
}
