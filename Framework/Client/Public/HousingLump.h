#pragma once
#include "GameObject.h"

namespace Client
{
	class CBuildManager;
	class CHousing;

	struct FHousingLumpDesc : public CGameObject::FGameObjectDesc
	{
		
	};

	class CHousingLump final : public CGameObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHousingLump() = default;

		virtual ~CHousingLump() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHousingLump> Create(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

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
	#pragma region Default
		/* �Ͽ�¡ �߰� */
		HRESULT AddHousing(std::tuple<int32, int32, int32> _tpIndex, std::shared_ptr<CHousing> _spHousing);

		/* �Ͽ�¡ ���� */
		HRESULT RemoveHousing(std::tuple<int32, int32, int32> _tpIndex);

		/* �Ͽ�¡ �˻� */
		std::shared_ptr<CHousing> FindHousing(std::tuple<int32, int32, int32> _tpIndex);

		/* �Ͽ�¡ ���� */
		HRESULT SaveHousingLump();
		// HRESULT SaveHousing(rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer, std::shared_ptr<CHousing> _spHousing);
	#pragma endregion

	public:
		/* ��ġ�� �ε��� ��ġ�� ���� Ÿ�Կ� �´� �������� ������ �����ϴ°�? */
		bool IsSatisfiedSupportingTheory(std::tuple<int32, int32, int32> _tpIndex, EBuildType _eBuildType);

	/********************
		Getter/Setter
	********************/
	public:
		const std::unordered_map <std::tuple<int32, int32, int32>, std::shared_ptr<CHousing>, FHashableTuple>& GetBuildIndices() { return m_umapBuildIndices; }

		std::shared_ptr<CHousing> GetFirstFoundation() { return m_umapBuildIndices[std::tuple(0, 0, 0)]; }

	/********************
		Data Members
	********************/
	private:
		/* ��� ���� */
		uint32 m_iNumFoundation = 0;

		/* 3���� �ε��� ���͸� Ű ������ ������ umap */
		std::unordered_map <std::tuple<int32, int32, int32>, std::shared_ptr<CHousing>, FHashableTuple> m_umapBuildIndices;

	private:
		/* ���� �Ŵ��� */
		shared_ptr<CBuildManager> m_spBuildManager = nullptr;
	};
}
