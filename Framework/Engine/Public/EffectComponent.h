#pragma once
#include "Component.h"

namespace Engine
{
	class CRenderManager;
	class CEffectShader;

	struct ENGINE_DLL FEffectComponentDesc : public CComponent::FComponentDesc
	{
		EEffectType m_eEffectType = EEffectType::ENUM_END;
	};

	class ENGINE_DLL CEffectComponent : public CComponent
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CEffectComponent() = default;

		virtual ~CEffectComponent() = default;

		/********************
			Events
		********************/
	public:
		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

		/********************
			Methods
		********************/
#pragma region �ʱ�ȭ
	public:
		/* Json ���� �ε� */
		virtual HRESULT LoadFromJsonFile(const wstring& _wstrJsonFileName) { return S_OK; }

		/* Json ���� ���� */
		virtual HRESULT WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer) { return S_OK; }

	private:
		/* ���� �ʱ�ȭ */
		virtual HRESULT InitializeBuffer() = 0;

		/* �ڵ鷯���� ����Ʈ ���̴� �������� */
		virtual HRESULT FindEffectShader() = 0;

#pragma region �Ϲ�
	public:
		/* ���� Ÿ�Կ� ���� �� ���� */
		float EaseByType(EEasingType _eEasingType, float& _fStart, float& _fEnd, float _fRatio, float _fPower = 0.f);
		Vector3 EaseByType(EEasingType _eEasingType, Vector3& _vStart, Vector3& _vEnd, float _fRatio, float _fPower = 0.f);
		Vector4 EaseByType(EEasingType _eEasingType, Vector4& _vStart, Vector4& _vEnd, float _fRatio, float _fPower = 0.f);

	private:
		/* ������Ʈ Ǯ���� ������ �� ���� */
		virtual void RecycleFromPool() = 0;

		/* ��Ȱ��ȭ �� ���� */
		virtual void NoneActivate() = 0;

		/* ���� �� ���� */
		virtual void EndLifeTime() = 0;

		/* ���� �� ���� */
		virtual void Loop() = 0;

		/* ����Ʈ ���� */
		virtual void PlayEffect(float _fDeltaSeconds) = 0;

		/* Ű������ ������ ���� */
		virtual void InterpolationBetweenKeyFrame(float _fDeltaSeconds) = 0;

		/* UVMove */
		virtual void UVMove(float _fDeltaSeconds) = 0;

		/********************
			Getter/Setter
		********************/
	public:
		void SetRenderType(ERenderType _eRenderType) { m_eRenderType = _eRenderType; }

		void SetTrackPosition(float _fTrackPosition) { m_fTrackPosition = _fTrackPosition; }
		float GetTrackPosition() { return m_fTrackPosition; }

		void SetPause() { m_bPause = !m_bPause; }
		bool GetPause() { return m_bPause; }

		/********************
			Data Members
		********************/
	protected:
		/* ����Ʈ ���̴� */
		weak_ptr<CEffectShader> m_wpEffectShader;

		/* Bind Variables */
		ComPtr<ID3DX11EffectVariable> m_cpVariable_MeshInfo = nullptr;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_cpVariable_Textures = nullptr;

		/* �н� �ε��� */
		uint16 m_iPassIndex = 0;

	protected:
		/* ����Ʈ Ÿ��*/
		EEffectType m_eEffectType = EEffectType::ENUM_END;

		/* Ʈ�� ������ */
		float m_fTrackPosition = 0.0f;

		/* �Ͻ� ���� */
		bool m_bPause = false;

		/* �� */
		bool m_bFinished = false;

		/* UVMove ���� */
		Vector2 m_vUVAcc = Vector2::Zero;

	protected:
		std::weak_ptr<CRenderManager> m_wpRenderManager;
	};
}

