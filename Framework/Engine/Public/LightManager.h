#pragma once
#include "Manager.h"
#define MAX_POINT_LIGHTS 8

namespace Engine
{
	//class CDepthCamera;
	class CEffectShader;
	class ENGINE_DLL CLightManager final : public CManager
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CLightManager() = default;

		virtual ~CLightManager() = default;

		/********************
			Events
		********************/
	public:
		HRESULT Initialize();

		void Tick();

		virtual void Release() override;

		/********************
			Methods
		********************/
	public:
		/// <summary>
		/// 방향성 조명 추가 함수
		/// </summary>
		/// <param name="_wstrLightName">조명 이름</param>
		/// <param name="_spLight">조명 정보</param>
		/// <returns>
		/// <para>조명 추가 성공 시 S_OK</para>
		/// <para>조명 추가 실패 시 ( 중복 이름, 조명 정보 잘못됨 ) E_FAIL</para>
		/// </returns>
		HRESULT AddLight(const std::wstring& _wstrLightName, shared_ptr<FDirectionalLight> _spLight, bool _bActive);

		/// <summary>
		/// 점 조명 추가 함수
		/// </summary>
		/// <param name="_wstrLightName">조명 이름</param>
		/// <param name="_spLight">조명 정보</param>
		/// <returns>
		/// <para>조명 추가 성공 시 S_OK</para>
		/// <para>조명 추가 실패 시 ( 중복 이름, 조명 정보 잘못됨 ) E_FAIL</para>
		/// </returns>
		HRESULT AddLight(const std::wstring& _wstrLightName, shared_ptr<FPointLight> _spLight, bool _bActive);

		/// <summary>
		/// 조명 제거 함수
		/// </summary>
		/// <param name="_wstrLightName">조명 이름</param>
		/// <param name="_eLightType">조명 타입</param>
		/// <returns>
		/// <para>조명 삭제 성공 시 S_OK</para>
		/// <para>조명 삭제 실패 시 ( 못찾음, 잘못된 조명 타입 ) E_FAIL</para>
		/// </returns>
		HRESULT RemoveLight(const std::wstring& _wstrLightName, ELightType _eLightType);

		/// <summary>
		/// 조명 활성화 제어 함수
		/// </summary>
		/// <param name="_wstrLightName">조명 이름</param>
		/// <param name="_eLightType">조명 타입</param>
		/// <param name="_bActive">목표 활성화 상태</param>
		/// <returns>
		/// <para>조명 제어 성공 시 S_OK</para>
		/// <para>조명 제어 실패 시 ( 못찾음, 잘못된 조명 타입 ) E_FAIL</para>
		/// </returns>
		HRESULT SetActiveLight(const std::wstring& _wstrLightName, ELightType _eLightType, bool _bActive);

		const FLightInfo& GetLightInfo() { return m_tLightInfo; };

		/// <summary>
		/// 전체 조명 제거 함수
		/// </summary>
		HRESULT ClearLight();

		/********************
			Data Members
		********************/
	private:
		void RefreshLightInfos();

		// 라이트 목록
		std::unordered_map<std::wstring, shared_ptr<FDirectionalLight>> m_umapDirLights;
		std::unordered_map<std::wstring, shared_ptr<FPointLight>>		m_umapPointLights;

		shared_ptr<FDirectionalLight> m_spCurrentDirLight;
		std::unordered_map<std::wstring, shared_ptr<FPointLight>>		m_umapCurrentPointLights;

		FAmbientLight m_tAmbientLight;
		FLightInfo m_tLightInfo;
	};
}