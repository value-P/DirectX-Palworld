#pragma once
#include "EffectComponent.h"

namespace Engine
{
	class CGameObject;
	class CInstancing;
	class CMaterial;
	class CModel;

	struct ENGINE_DLL FParticleComponentDesc : public FEffectComponentDesc
	{

	};

	class ENGINE_DLL CParticleComponent : public CEffectComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CParticleComponent() = default;

		virtual ~CParticleComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CParticleComponent> Create(const std::shared_ptr<FParticleComponentDesc>& _spParticleComponentDesc, FParticleInfo _tParticleInfo);

		static std::shared_ptr<CParticleComponent> CreateFromJson(const std::shared_ptr<FParticleComponentDesc>& _spParticleComponentDesc, const std::wstring& _wstrJsonFileName);

	public:
		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTickComponent(float _fDeltaSeconds) override;

		virtual int32 TickComponent(float _fDeltaSeconds) override;

		virtual int32 PostTickComponent(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Render(int32 _iShaderPath) override;

		virtual void Release() override;

		virtual HRESULT BindVariable() override;

	/********************
		Methods
	********************/
	public:
		/* Json 파일 로드 */
		virtual HRESULT LoadFromJsonFile(const wstring& _wstrJsonFileName) override;

		/* Json 파일 저장 */
		virtual HRESULT WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer) override;

	private:
		/* 버퍼 초기화 */
		virtual HRESULT InitializeBuffer() override;

		/* 핸들러에서 이펙트 셰이더 가져오기 */
		virtual HRESULT FindEffectShader() override;

		/* 텍스처 가져오기 */
		HRESULT FindShaderResourceViews();

		/* 인스턴스 값 초기화 */
		virtual void InitializeInstanceValue();

	public:
		/* 오브젝트 풀에서 꺼내올 시 실행될 함수 */
		virtual void RecycleFromPool() override;

		/* 비활성화 시 실행 */
		virtual void NoneActivate() override;

		/* 종료 시 실행 */
		virtual void EndLifeTime() override;

		/* 루프 시 실행 */
		virtual void Loop() override;

		/* 이펙트 진행 */
		virtual void PlayEffect(float _fDeltaSeconds) override;

		/* 키프레임 사이의 보간 */
		virtual void InterpolationBetweenKeyFrame(float _fDeltaSeconds) override;

		/* UVMove */
		virtual void UVMove(float _fDeltaSeconds) override;

		/* 키프레임 추가 */
		void AddKeyFrame(FParticleKeyFrame _tParticleKeyFrame);

	public:
		void TickIntegrated(float _fDeltaSeconds);
		void TickSprite(D3D11_MAPPED_SUBRESOURCE& _tSubResource, uint32 _iIndex);

	public:
		void ResetInstanceBuffer();
		void ResetInstanceBufferByBonePosition(std::shared_ptr<CGameObject> _spGameObject, std::shared_ptr<CModel> _spModel);
		void ResetAllInstanceValue();
		void ResetInstanceValue(D3D11_MAPPED_SUBRESOURCE& _tSubResource, uint32 _iIndex);
		void ResetOwnerPosition();

	/********************
		Getter / Setter
	********************/
	public:
		/* 키프레임 벡터 */
		std::vector<FParticleKeyFrame>& GetKeyFrames() { return m_tParticleInfo.m_vecKeyFrames; }

		/* 부모 오브젝트 위치 반환 */
		Vector3 GetOwnerPositionV3();
		Vector4 GetOwnerPosition();

		/* 부모 오브젝트 행렬 반환 */
		Matrix GetOwnerMatrix();

		/* 파티클 Info */
		void SetParticleInfo(const FParticleInfo& _tParticleInfo) { m_tParticleInfo = _tParticleInfo; }
		FParticleInfo& GetParticleInfo() { return m_tParticleInfo; }

		/* 현재 키프레임 인덱스 */
		void SetCurKeyFrameIndex(int32 _iCurKeyFrameIndex) { m_iCurKeyFrame = _iCurKeyFrameIndex; }
		uint32 GetCurKeyFrameIndex() { return m_iCurKeyFrame; }

		/* 키프레임 */
		void SetKeyFrameValue(const FParticleKeyFrame& _tParticleKeyFrame, int32 _iIndex);
		const std::optional<FParticleKeyFrame> GetKeyFrameValue(int32 _iIndex);

		void SetEffectShader(const std::wstring& _wstrEffectShaderName);
		void SetTexture(const std::wstring& _wstrTextureName, ETextureType _eTextureType);

	/********************
		Data Members
	********************/
	protected:
	#pragma region 컴 객체
		/* 디바이스 */
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		/* 컨텍스트*/
		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		/* 인스턴스 버퍼 */
		std::shared_ptr<CInstancing> m_spInstancing;

		/* 텍스처 */
		vector<ID3D11ShaderResourceView*> m_vecSRVs;

		/* Bind */
		FEffectInfo_Particle m_tEffectInfoParticle = {};

		/* 디졸브 텍스처 */
		ComPtr<ID3D11ShaderResourceView> m_cpDissolveTexture = nullptr;
#pragma endregion

	protected:
	#pragma region 인스턴스 밸류
		/* 스피드 */
		std::vector<float> m_vecSpeed;

		/* 라이프 타임 */
		std::vector<float> m_vecLifeTime;

		/* 누적 라이프 타임 */
		std::vector<float> m_vecLifeTimeAcc;

		/* 스프라이트 누적 라이프 타임 */
		std::vector<float> m_vecSpriteLifeTimeAcc;

		/* 알파 감소 누적 값 */
		std::vector<float> m_vecAlphaDecreaseAcc;

		/* 로테이션 */
		std::vector<Vector3> m_vecRotation;

		/* 인스턴스 각자의 방향 */
		std::vector<Vector4> m_vecDirection;
#pragma endregion

	protected:
	#pragma region 파티클 Base
		/* 이펙트 발생 근원지 */
		Vector3 m_vOriginPos = Vector3::Zero;

		/* 파티클 Info */
		FParticleInfo m_tParticleInfo;

		/* 현재 키프레임 인덱스 */
		int32 m_iCurKeyFrame = 0;

		/* 보간 완료된 데이터 */
		FParticleKeyFrame m_tLerpedData;
#pragma endregion
	};
}

