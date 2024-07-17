#pragma once

namespace Client
{
	class CPlayer;
	class CItem;
	class CArchitecture;

	/********************
		Descriptor
	********************/
	struct FTechTreeSystemDesc final
	{
	};

	class CTechTreeSystem final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTechTreeSystem() = default;

		virtual ~CTechTreeSystem() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTechTreeSystem> Create(const std::shared_ptr<FTechTreeSystemDesc>& _spTechTreeSystemDesc);

		HRESULT Initialize(const std::shared_ptr<FTechTreeSystemDesc>& _spTechTreeSystemDesc);

		HRESULT BeginPlay();

		int32 PreTick(float _fDeltaSeconds);

		int32 Tick(float _fDeltaSeconds);
		
		int32 PostTick(float _fDeltaSeconds);

		HRESULT EndPlay();

		void Release();
		
	/********************
		Getter/Setter
	********************/

	/********************
		Methods
	********************/
	public:
		void UnlockItem(EItemType _eItemType);

		void UnlockArchitecture(EArchitectureType _eArchitectureType);

	/********************
		Data Members
	********************/
	private:
		std::weak_ptr<CPlayer> m_wpPlayer;

		int32 m_iCurPlayerLevel = 0;

		int32 m_iPrePlayerLevel = 0;
	};
}
