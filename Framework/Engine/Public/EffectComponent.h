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
#pragma region 초기화
	public:
		/* Json 파일 로드 */
		virtual HRESULT LoadFromJsonFile(const wstring& _wstrJsonFileName) { return S_OK; }

		/* Json 파일 저장 */
		virtual HRESULT WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer) { return S_OK; }

	private:
		/* 버퍼 초기화 */
		virtual HRESULT InitializeBuffer() = 0;

		/* 핸들러에서 이펙트 셰이더 가져오기 */
		virtual HRESULT FindEffectShader() = 0;

#pragma region 일반
	public:
		/* 보간 타입에 따라 값 보간 */
		float EaseByType(EEasingType _eEasingType, float& _fStart, float& _fEnd, float _fRatio, float _fPower = 0.f);
		Vector3 EaseByType(EEasingType _eEasingType, Vector3& _vStart, Vector3& _vEnd, float _fRatio, float _fPower = 0.f);
		Vector4 EaseByType(EEasingType _eEasingType, Vector4& _vStart, Vector4& _vEnd, float _fRatio, float _fPower = 0.f);

	private:
		/* 오브젝트 풀에서 꺼내올 시 실행 */
		virtual void RecycleFromPool() = 0;

		/* 비활성화 시 실행 */
		virtual void NoneActivate() = 0;

		/* 종료 시 실행 */
		virtual void EndLifeTime() = 0;

		/* 루프 시 실행 */
		virtual void Loop() = 0;

		/* 이펙트 진행 */
		virtual void PlayEffect(float _fDeltaSeconds) = 0;

		/* 키프레임 사이의 보간 */
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
		/* 이펙트 셰이더 */
		weak_ptr<CEffectShader> m_wpEffectShader;

		/* Bind Variables */
		ComPtr<ID3DX11EffectVariable> m_cpVariable_MeshInfo = nullptr;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_cpVariable_Textures = nullptr;

		/* 패스 인덱스 */
		uint16 m_iPassIndex = 0;

	protected:
		/* 이펙트 타입*/
		EEffectType m_eEffectType = EEffectType::ENUM_END;

		/* 트랙 포지션 */
		float m_fTrackPosition = 0.0f;

		/* 일시 정지 */
		bool m_bPause = false;

		/* 끝 */
		bool m_bFinished = false;

		/* UVMove 누적 */
		Vector2 m_vUVAcc = Vector2::Zero;

	protected:
		std::weak_ptr<CRenderManager> m_wpRenderManager;
	};
}

