#pragma once

namespace Client
{
	class CHabitatCache final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHabitatCache() = default;

		virtual ~CHabitatCache() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHabitatCache> Create(const std::wstring& _wstrJsonFileName);

	/********************
		Methods
	********************/
	private:
		HRESULT LoadHabitatDataFromJson(const std::wstring& _wstrJsonFileName);
		
	/********************
		Data Members
	********************/
	private:/*
		std::vector<FHabitatTreeOrRockCache>		m_vecHabitatTreeOrRockCaches;

		std::vector<FHabitatBranchOrStoneCache>		m_vecHabitatBranchOrStoneCaches;

		std::vector<FHabitatPalCache>				m_vecHabitatPalCaches;

		std::vector<FHabitatArchitectureCache>		m_vecHabitatArchitectureCaches;*/

		std::vector<FHabitatSpotLightCache>			m_vecHabitatSpotLightCaches;

		std::vector<FHabitatPointLightCache>		m_vecHabitatPointLightCaches;

		std::vector<FHabitatDirectionalLightCache>  m_vecHabitatDirectionalLightCaches;

		std::vector<FHabitatCameraFrustumCache>		m_vecHabitatCameraFrustumCaches;
	};
}
