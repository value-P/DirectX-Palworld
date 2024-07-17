#pragma once
#include "Manager.h"

namespace Engine
{
	class CPipelineManager;
	class CLightManager;
	class CComponent;
	class CEffectShader;
	class CRectBuffer;
	class CTreeNode;
	class CRectComponent;
	class CCameraManager;
	class CTimerManager;
	class CInputManager;
	class CMeshHandler;
	class CShaderHandler;
	class CModel;

	class ENGINE_DLL CRenderManager final : public CManager
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CRenderManager() = default;

		virtual ~CRenderManager() = default;

		/********************
			Events
		********************/
	public:
		HRESULT Initialize();

		int32 Tick(float _fDeltaSeconds);

		void Render();

		virtual void Release() override;

		/********************
			Methods
		********************/
	public:
		HRESULT AddComponent(const std::shared_ptr<CComponent>& _spComponent, ERenderType _eRenderType);

		HRESULT AddTreeNode(const std::shared_ptr<CTreeNode>& _spTreeNode);

		void SetActive(bool _bActive) { m_bActive = _bActive; }

	private:
		void RenderPre();
		void RenderNblend();
		void RenderNblend_NonCull();
		void RenderLblend();
		void RenderLblend_NonCull();
		void RenderLblend_Add();
		void RenderLblend_Add_NonCull();
		void RenderLblend_Reverse();
		void RenderLblend_Reverse_NonCull();
		void RenderPbr();
		void RenderNLight();
		void RenderNLight_NonCull();
		void RenderBlend();
		void RenderBlend_NonCull();
		void RenderBlend_Add();
		void RenderBlend_Add_NonCull();
		void RenderBlend_Reverse();
		void RenderBlend_Reverse_NonCull();

		void RenderGlow();
		void RenderDistortion();
		void RenderUi();
		void RenderMosaic();
		void RenderPost();
		void RenderPostEffect();
		void RenderPostUI();
		void RenderDebug();

		/********************
			Data Members
		********************/
	private:
		bool m_bActive = false;

		ComPtr<ID3D11Device> m_cpDevice = nullptr;
		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		// Buffers
		shared_ptr<CRectBuffer> m_spRectBuffer = nullptr;

		// Sky
		shared_ptr<CModel> m_spSkyDome = nullptr;

		// Shaders
		shared_ptr<CEffectShader> m_spSkyShader = nullptr;
		shared_ptr<CEffectShader> m_spPbrShader = nullptr;
		shared_ptr<CEffectShader> m_spEffectShader = nullptr;

		// Bind
		FFrameInfo m_tFrameInfo = {};
		FPBR m_tPBR = {};
		FEffectInfo_Deffered m_tEffectInfoDeffered = {};

		// Bind Variables
		ComPtr<ID3DX11EffectVariable> m_cpVariable_Sky;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_cpVariable_TexturesSky;
		ComPtr<ID3DX11EffectVariable> m_cpVariable_PBR;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_cpVariable_TexturesPBR;
		ComPtr<ID3DX11EffectVariable> m_cpVariable_EffectDeffered;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_cpVariable_TexturesEffectDeffered;

		// SRVs
		vector<ID3D11ShaderResourceView*> m_vecSRVs_PBR;
		vector<ID3D11ShaderResourceView*> m_vecSRVs_Effect;

		std::array<std::list<std::shared_ptr<CComponent>>, (uint8)ERenderType::ENUM_END> m_arrComponents;

		// Managers
		std::weak_ptr<CPipelineManager> m_wpPipelineManager;
		std::weak_ptr<CCameraManager> m_wpCameraManager;
		std::weak_ptr<CLightManager> m_wpLightManager;
		std::weak_ptr<CTimerManager> m_wpTimeManager;
		std::weak_ptr<CMeshHandler> m_wpMeshHandler;
		std::weak_ptr<CShaderHandler> m_wpShaderHandler;

#ifdef _DEBUG
		std::weak_ptr<CInputManager> m_wpInputManager;
#endif
		// 디버그 렌더할 옥트리 노드들
		std::list<std::shared_ptr<CTreeNode>> m_lstTreeNodes;

	private:
		// 디스토션 위해 추가. 04.21 동연
		ComPtr<ID3D11ShaderResourceView> m_cpNoiseTexture = nullptr;
	};
}
