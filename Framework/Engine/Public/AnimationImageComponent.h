#pragma once
#include "RectComponent.h"

namespace Engine
{
	class ENGINE_DLL CAnimationImageComponent : public CRectComponent
	{
		/********************
			Description
		********************/
	public:
		struct ENGINE_DLL FAnimationImageComponentDesc : public FRectComponentDesc
		{
			std::wstring m_wstrTextureFileName = L"";

			FImageAnimationFrameDesc m_fImageAnimationDesc;

			bool m_bLoop = false;

			bool m_bTool = false;

			bool m_bRative = false;
		};

		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CAnimationImageComponent() = default;

		virtual ~CAnimationImageComponent() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CAnimationImageComponent> Create(const std::shared_ptr<FAnimationImageComponentDesc>& _spAnimationImageComponentDesc);

		static std::shared_ptr<CAnimationImageComponent> CreateFromJson(const std::shared_ptr<FAnimationImageComponentDesc>& _spAnimationImageComponentDesc, rapidjson::Document& _doc, int32& iPanelIndex, int32& iComponentIndex, int32& iKeyFrameIndex);

		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc) override;

	public:
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
		// map 컨테이너에 키프레임을 추가합니다.
		HRESULT AddKeyFrame(FImageAnimationFrameDesc _FImageAnimationDesc);

		// map 컨테이너에서 특정 키프레임을 삭제합니다.
		HRESULT SubtractKeyFrame(const int _iIndex);

		// map 컨테이너에 있는 특정 프레임을 바꿔줍니다.
		HRESULT ReplaceKeyFrame(FImageAnimationFrameDesc _FImageAnimationDesc);

		// map 컨테이너에 있는 프레임들을 기준좌표를 기준으로 절대 좌표를 바꿔줍니다.
		HRESULT CaculateKeyFramesRelativePosition();

		// 맵에 저장된 키프레임들을 보간해줍니다.
		void CalculateLerp(float _fDeltaSeconds);

		void ResetWorldPosition(const Vector2& _vCenter);

		pair<float, FImageAnimationFrameDesc> FindKeyFrame(const int _iIndex);

		static bool CompareKey(pair<float, FImageAnimationFrameDesc>& _prFirst, pair<float, FImageAnimationFrameDesc>& _prSecond);

		void ResetFrame();

		void SetStartEndIndex(int32 _iStartIndex, int32 _iEndIndex);

		pair<int32, int32> GetStartEndIndex() { return { m_iStartIndex, m_iEndIndex }; }

		void ResetAnimationValues();

		// 프레임 애니메이션 계산을 위한 함수입니다.
		void CaculateFrameAnimation(float _fDeltaSeconds);

		void SetRowCoulmn(int32 _iRow, int32 _iColumn, float _fNextTime);
	public:
		/* 보간 타입에 따라 값 보간 */
		float EaseByType(EEasingType _eEasingType, float& _fStart, float& _fEnd, float _fRatio, float _fPower = 0.f);

		Vector3 EaseByType(EEasingType _eEasingType, Vector3& _vStart, Vector3& _vEnd, float _fRatio, float _fPower = 0.f);

		Vector4 EaseByType(EEasingType _eEasingType, Vector4& _vStart, Vector4& _vEnd, float _fRatio, float _fPower = 0.f);

		void DelayTexture();

		/********************
			Getter/Setter
		********************/
	public:
		void SetAnimationRunTime(float _fAnimationRunTime) { m_fAnimationRunTime = _fAnimationRunTime; }

		float GetAnimationRunTime() const { return m_fAnimationRunTime; }

		float GetKeyFrameRunTime() const { return m_fKeyFrameRunTime; }

		float GetDuration() const { return m_fDuration; }

		void SetLoop(bool _bLoop) { m_bLoop = _bLoop; };

		bool GetLoop() const { return m_bLoop; }

		void SetReset(bool _bReset)
		{
			m_bReset = _bReset;
			if (m_bReset)
			{
				m_fAnimationRunTime = 0.0f;
				m_fKeyFrameRunTime = 0.0f;
				m_iCurIndex = 0;
			}
		}

		void SetStartReset(bool _bReset);

		bool GetReset() const { return m_bReset; }

		void SetIndex(uint32 _iIndex) { m_iCurIndex = _iIndex; }

		uint32 GetIndex() const { return m_iCurIndex; }

		std::vector<pair<float, FImageAnimationFrameDesc>>* GetKeyFrames() { return &m_vecKeyFrames; }

		void SetTextureFileName(const std::wstring& _wstrTextureName);

		void SetTextureFileName(const std::wstring& _wstrTextureName, bool _bDelay);

		std::wstring GetTextureFileName() const { return m_wstrTextureFileName; }

		void SetEasingType(EEasingType _eEasingType) { m_eEasingType = _eEasingType; }

		EEasingType GetEasingType() const { return m_eEasingType; }

		void SetFinish(bool _bFinish) { m_bFinish = _bFinish; }

		const bool IsFinish() const { return m_bFinish; }

		void SetStartIndex(int32 _iStartIndex);

		const int32 GetStartIndex() const { return m_iStartIndex; }

		void SetEndIndex(int32 _iEndIndex);

		const int32 GetEndIndex() const { return m_iEndIndex; }

		void SetOriginStartIndex(int32 _iOriginStartIndex) { m_iOriginStartIndex = _iOriginStartIndex; }

		const int32 GetOriginStartIndex() const { return m_iOriginStartIndex; }

		void SetOriginEndIndex(int32 _iOriginEndIndex) { m_iOriginEndIndex = _iOriginEndIndex; }

		const int32 GetOriginEndIndex() const { return m_iOriginEndIndex; }

		void SetLoopCount(int32 _iLoopCount) { m_iLoopCount = _iLoopCount; }

		const int32 GetLoopCount() const { return m_iLoopCount; }

		void SetRow(int32 _iRow) { m_iRow = _iRow; }

		const int32 GetRow() const { return m_iRow; }

		void SetColumn(int32 _iColumn) { m_iColumn = _iColumn; }

		const int32 GetColoumn() const { return m_iColumn; }

		void SetNextTime(float _fNextTime) { m_fNextTime = _fNextTime; }

		const float GetNextTime() const { return m_fNextTime; }

		/********************
			Data Members
		********************/
	private:
		std::wstring m_wstrTextureFileName = L"";

	private:
		// key값은 시간을 나타냅니다.
		// 설정한 키프레임의 값들을 저장합니다.
		std::vector<pair<float, FImageAnimationFrameDesc>> m_vecKeyFrames;
		// 애니메이션이 수행되고 있는 시간을 저장합니다.
		float m_fAnimationRunTime = 0.0f;
		// 애니메이션 구간의 전체 시간을 저장합니다.
		float m_fDuration = 0.0f;
		// 애니메이션 구간별 수행 시간을 저장합니다.
		float m_fKeyFrameRunTime = 0.0f;
		//애니메이션 루프 여부를 저장합니다.
		bool m_bLoop = false;
		// 애니메이션 리셋/ 재생 여부를 저장합니다.
		bool m_bReset = false;
		// 현재 인덱스를 저장합니다.
		int32 m_iCurIndex = 0;
		// 보간 타입을 설정합니다.
		EEasingType m_eEasingType = EEasingType::EASE_LINEAR;
		// 루프가 아닐 시 끝나는 지점을 체크합니다.
		bool m_bFinish = false;
		// 애니메이션의 시작 인덱스를 지정합니다.
		int32 m_iStartIndex = 0;
		// 애니메이션의 끝 인덱스를 지정합니다.
		int32 m_iEndIndex = 1;
		// 초기 시작 인덱스를 저장합니다.
		int32 m_iOriginStartIndex = 0;
		// 초기 끝 인덱스를 저장합니다.
		int32 m_iOriginEndIndex = 1;
		// 루프 카운트를 저장합니다.
		int32 m_iLoopCount = 0;
		// 텍스처를 바꿀때 딜레이를 주기 위한 변수입니다.
		bool m_bDelay = false;

		// 직접 만드는 애니메이션인지 아틀라스 애니메이션인지의 여부를 저장합니다.
		// 행
		int32 m_iRow = 1;
		// 열
		int32 m_iColumn = 1;
		// 넘어가는 시간
		float m_fNextTime = 0.0f;

		float m_fDeltaSeconds = 0.0f;

		// 셰이더에 넘겨줄 값
		FUiInfo_AnimationImage m_tShaderVariable_AnimationImage = {};

		// Bind Variables
		ComPtr<ID3DX11EffectVariable> m_cpVariable_UiInfo_AnimationImage;
	};
}
