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
		/// ���⼺ ���� �߰� �Լ�
		/// </summary>
		/// <param name="_wstrLightName">���� �̸�</param>
		/// <param name="_spLight">���� ����</param>
		/// <returns>
		/// <para>���� �߰� ���� �� S_OK</para>
		/// <para>���� �߰� ���� �� ( �ߺ� �̸�, ���� ���� �߸��� ) E_FAIL</para>
		/// </returns>
		HRESULT AddLight(const std::wstring& _wstrLightName, shared_ptr<FDirectionalLight> _spLight, bool _bActive);

		/// <summary>
		/// �� ���� �߰� �Լ�
		/// </summary>
		/// <param name="_wstrLightName">���� �̸�</param>
		/// <param name="_spLight">���� ����</param>
		/// <returns>
		/// <para>���� �߰� ���� �� S_OK</para>
		/// <para>���� �߰� ���� �� ( �ߺ� �̸�, ���� ���� �߸��� ) E_FAIL</para>
		/// </returns>
		HRESULT AddLight(const std::wstring& _wstrLightName, shared_ptr<FPointLight> _spLight, bool _bActive);

		/// <summary>
		/// ���� ���� �Լ�
		/// </summary>
		/// <param name="_wstrLightName">���� �̸�</param>
		/// <param name="_eLightType">���� Ÿ��</param>
		/// <returns>
		/// <para>���� ���� ���� �� S_OK</para>
		/// <para>���� ���� ���� �� ( ��ã��, �߸��� ���� Ÿ�� ) E_FAIL</para>
		/// </returns>
		HRESULT RemoveLight(const std::wstring& _wstrLightName, ELightType _eLightType);

		/// <summary>
		/// ���� Ȱ��ȭ ���� �Լ�
		/// </summary>
		/// <param name="_wstrLightName">���� �̸�</param>
		/// <param name="_eLightType">���� Ÿ��</param>
		/// <param name="_bActive">��ǥ Ȱ��ȭ ����</param>
		/// <returns>
		/// <para>���� ���� ���� �� S_OK</para>
		/// <para>���� ���� ���� �� ( ��ã��, �߸��� ���� Ÿ�� ) E_FAIL</para>
		/// </returns>
		HRESULT SetActiveLight(const std::wstring& _wstrLightName, ELightType _eLightType, bool _bActive);

		const FLightInfo& GetLightInfo() { return m_tLightInfo; };

		/// <summary>
		/// ��ü ���� ���� �Լ�
		/// </summary>
		HRESULT ClearLight();

		/********************
			Data Members
		********************/
	private:
		void RefreshLightInfos();

		// ����Ʈ ���
		std::unordered_map<std::wstring, shared_ptr<FDirectionalLight>> m_umapDirLights;
		std::unordered_map<std::wstring, shared_ptr<FPointLight>>		m_umapPointLights;

		shared_ptr<FDirectionalLight> m_spCurrentDirLight;
		std::unordered_map<std::wstring, shared_ptr<FPointLight>>		m_umapCurrentPointLights;

		FAmbientLight m_tAmbientLight;
		FLightInfo m_tLightInfo;
	};
}