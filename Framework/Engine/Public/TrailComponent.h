#pragma once
#include "Component.h"

namespace Engine
{
	class CRenderManager;

	class CEffectShader;
	class CTrailBuffer;

	struct ENGINE_DLL FTrailComponentDesc : public CComponent::FComponentDesc
	{
		uint32 m_iNumRects = 0;

		float m_fMaxLifeTime = 1.f;

		Vector4 m_vColor = Vector4::Zero;

		std::wstring m_wstrEffectShaderName = L"";

		bool m_bUseDiffuse = false;
		bool m_bUseNoise = false;
		bool m_bUseMask = false;
		bool m_bUseDistortion = false;
		bool m_bUseDissolve = false;

		std::wstring m_wstrDiffuseTextureName = L"";
		std::wstring m_wstrNoiseTextureName = L"";
		std::wstring m_wstrMaskTextureName = L"";
		std::wstring m_wstrDistortionTextureName = L"";
		std::wstring m_wstrDissolveTextureName = L"";
	};

	class ENGINE_DLL CTrailComponent final : public CComponent
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CTrailComponent() = default;

		virtual ~CTrailComponent() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CTrailComponent> Create(const std::shared_ptr<FComponentDesc>& _spComponentDesc);

	public:
		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

	private:
		HRESULT InitializeTrailBuffer();

		virtual HRESULT BindVariable() override;

		/********************
			Methods
		********************/
	public:
		void TickTrail(const Vector3& _vStartPos, const Vector3 _vEndPos);

		void ClearRectList(const Vector3& _vStartPos, const Vector3 _vEndPos);

		/********************
			Getter/Setter
		********************/
	public:


		/********************
			Data Members
		********************/
	protected:
#pragma region ����̽�
		/* ����̽� */
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		/* ���ؽ�Ʈ*/
		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;
#pragma endregion
		/* Ʈ���� ���� */
		std::shared_ptr<CTrailBuffer> m_spTrailBuffer = nullptr;

		/* ����Ʈ ���̴� */
		std::shared_ptr<CEffectShader> m_spEffectShader = nullptr;

		/* Bind Variables */
		ComPtr<ID3DX11EffectVariable> m_cpVariable_EffectInfo = nullptr;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_cpVariable_Textures = nullptr;

		/* �н� �ε��� */
		uint16 m_iPassIndex = 0;

	protected:
		/* �ִ� �簢�� ���� */
		uint32 m_iNumRects = 0;

		/* Ʈ���� Start, End ���� ť */
		std::list<pair<Vector3, Vector3>> m_lstTrailPositions;

		/* Ʈ���� �簢�� �ִ� ���ӽð� */
		float m_fMaxLifeTime = 1.f;

		/* �ӽ� */
		float m_fTempAcc = 0.f;

		/* Ʈ���� �簢�� LifeTime */
		std::vector<float> m_vecLifeTime;

		/* ��ǻ�� �ؽ�ó�� ������� �ʴ� ����� �÷� */
		Vector4 m_vColor = Vector4::Zero;

	protected:
		// Bind
		FEffectInfo_Trail m_tEffectInfoTrail = {};

		/* �ؽ�ó */
		bool m_bUseDiffuse = false;
		bool m_bUseNoise = false;
		bool m_bUseMask = false;
		bool m_bUseDistortion = false;
		bool m_bUseDissolve = false;

		vector<ID3D11ShaderResourceView*> m_vecSRVs;

	protected:
		std::weak_ptr<CRenderManager> m_wpRenderManager;
	};
}
