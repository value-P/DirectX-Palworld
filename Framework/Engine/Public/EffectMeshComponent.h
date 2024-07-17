#pragma once
#include "EffectComponent.h"

namespace Engine
{
	class CModel;
	class CMaterial;

	struct ENGINE_DLL FEffectMeshComponentDesc : public FEffectComponentDesc
	{
		std::wstring m_wstrJsonFileName = L""; // Json으로 로드 안하고, 클라에서 직접 만드는 경우 비워둬도 됩니다.

		std::wstring m_wstrModelFilePath = L"";

		Matrix	m_matPivot = Matrix::Identity;
	};

	class ENGINE_DLL CEffectMeshComponent : public CEffectComponent
	{
		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CEffectMeshComponent() = default;

		virtual ~CEffectMeshComponent() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CEffectMeshComponent> Create(const std::shared_ptr<FEffectMeshComponentDesc>& _spEffectMeshComponentDesc, FEffectMeshInfo _tEffectMeshInfo);

		static std::shared_ptr<CEffectMeshComponent> CreateFromJson(const std::shared_ptr<FEffectMeshComponentDesc>& _spEffectMeshComponentDesc, const std::wstring& _wstrJsonFileName);

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
#pragma region 초기화
	public:
		/* Json 파일 로드 */
		virtual HRESULT LoadFromJsonFile(const wstring& _wstrJsonFileName) override;

		/* Json 파일 저장 */
		virtual HRESULT WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer) override;

	protected:
		/* 버퍼 초기화 */
		virtual HRESULT InitializeBuffer() override;

		/* 핸들러에서 이펙트 셰이더 가져오기 */
		virtual HRESULT FindEffectShader() override;

		/* 텍스처 가져오기 */
		HRESULT FindShaderResourceViews();

#pragma region 일반
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

		/* 트레일 진행 */
		void PlayTrail(float _fDeltaSeconds);

		/* 키프레임 사이의 보간 */
		virtual void InterpolationBetweenKeyFrame(float _fDeltaSeconds) override;

		/* UVMove */
		virtual void UVMove(float _fDeltaSeconds) override;

		/* 스프라이트 모드 */
		void TickSprite(float _fDeltaSeconds);

		/* 키프레임 추가 */
		void AddKeyFrame(FEffectMeshKeyFrame _tEffectMeshKeyFrmae);

		/********************
			Getter / Setter
		********************/
	public:
		/* 키프레임 벡터 */
		std::vector<FEffectMeshKeyFrame>& GetKeyFrames() { return m_tEffectMeshInfo.m_vecKeyFrames; }

		/* 이펙트 메시 Info */
		void SetEffectMeshInfo(const FEffectMeshInfo& _tEffectMeshInfo) { m_tEffectMeshInfo = _tEffectMeshInfo; }
		FEffectMeshInfo& GetEffectMeshInfo() { return m_tEffectMeshInfo; }

		/* 현재 키프레임 인덱스 */
		void SetCurKeyFrameIndex(int32 _iCurKeyFrameIndex) { m_iCurKeyFrame = _iCurKeyFrameIndex; }
		uint32 GetCurKeyFrameIndex() { return m_iCurKeyFrame; }

		/* 키프레임 */
		void SetKeyFrameValue(const FEffectMeshKeyFrame& _tEffectMeshKeyFrame, int32 _iIndex);
		const std::optional<FEffectMeshKeyFrame> GetKeyFrameValue(int32 _iIndex);

		void SetEffectShader(const std::wstring& _wstrEffectShaderName);
		void SetTexture(const std::wstring& _wstrTextureName, ETextureType _eTextureType);

		/********************
			Data Members
		********************/
	protected:
		/* Json 파일 */
		std::wstring m_wstrJsonFileName = L"";

		/* 모델 파일 경로 */
		wstring m_wstrModelFilePath = L"";

		/* 피벗 매트릭스(이펙트는 항등) */
		Matrix m_matPivot = Matrix::Identity;

		/* 메쉬 트레일 연습 */
		std::list<Matrix> m_lstMeshMatrices;

		float m_fTrailTimeAcc = 0.f;

	protected:
		/* 모델 */
		std::shared_ptr<CModel> m_spModel;

		/* Bind */
		FMeshInfo_Effect m_tMeshInfoEffect = {};

		/* Bind Variables */
		ComPtr<ID3DX11EffectMatrixVariable> m_cpVariable_Transform = nullptr;

		/* 텍스처 */
		vector<ID3D11ShaderResourceView*> m_vecSRVs;

		/* 매터리얼 */// 노멀, 매탈릭 등을 위해 추가 - 준영이의 요청 -
		std::shared_ptr<CMaterial> m_spMaterial;

	protected:
		/* 이펙트 발생 근원지 */
		Vector3 m_vOriginPos = Vector3::Zero;

		/* 이펙트 메시 Info */
		FEffectMeshInfo m_tEffectMeshInfo;

		/* 현재 키프레임 인덱스 */
		int32 m_iCurKeyFrame = 0;

		/* 보간 완료된 데이터 */
		FEffectMeshKeyFrame m_tLerpedData;

		/* 스프라이트 누적 라이프 타임 */
		float m_fSpriteLifeTimeAcc = 0.f;

		/* 스프라이트 UV */
		Vector2 m_vMinUV = Vector2::Zero;
		Vector2 m_vMaxUV = Vector2::One;
	};
}