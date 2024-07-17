#pragma once
#include "EffectMeshComponent.h"

namespace Engine 
{
	class CModel;
	class CMaterial;
	class CInstanceEffectModel;

	struct ENGINE_DLL FInstanceEffectMeshComponentDesc : public FEffectMeshComponentDesc
	{

	};

	class ENGINE_DLL CInstanceEffectMeshComponent : public CEffectMeshComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CInstanceEffectMeshComponent() = default;

		virtual ~CInstanceEffectMeshComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CInstanceEffectMeshComponent> Create(const std::shared_ptr<FInstanceEffectMeshComponentDesc>& _spInstanceEffectMeshComponentDesc, FEffectMeshInfo _tEffectMeshInfo, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo);

		static std::shared_ptr<CInstanceEffectMeshComponent> CreateFromJson(const std::shared_ptr<FInstanceEffectMeshComponentDesc>& _spInstanceEffectMeshComponentDesc, const std::wstring& _wstrJsonFileName);

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

	private:
		/* 버퍼 초기화 */
		virtual HRESULT InitializeBuffer() override;

		/* 인스턴스 값 초기화 */
		virtual void InitializeInstanceValue();

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

		/* UVMove */
		virtual void UVMove(float _fDeltaSeconds) override;

	private:
		void TickIntegrated(float _fDeltaSeconds);

	private:
		void ResetInstanceBuffer();
		void ResetAllInstanceValue();
		void ResetInstanceValue(D3D11_MAPPED_SUBRESOURCE& _tSubResource, uint32 _iIndex);
	/********************
		Getter / Setter
	********************/
	public:
		void SetInstanceEffectMeshInfo(FInstanceEffectMeshInfo _tInstanceEffectMeshInfo) { m_tInstanceEffectMeshInfo = _tInstanceEffectMeshInfo; }
		FInstanceEffectMeshInfo& GetInstanceEffectMeshInfo() { return m_tInstanceEffectMeshInfo; }

	/********************
		Data Members
	********************/
	protected:
	#pragma region 디바이스
		/* 디바이스 */
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		/* 컨텍스트*/
		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;
	#pragma endregion

	protected:
	#pragma region 인스턴스 밸류
		/* 스피드 */
		std::vector<float> m_vecSpeed;

		/* 라이프 타임 */
		std::vector<float> m_vecLifeTime;

		/* 누적 라이프 타임 */
		std::vector<float> m_vecLifeTimeAcc;

		/* 로테이션 */
		std::vector<Vector3> m_vecRotation;

		/* 인스턴스 각자의 방향 */
		std::vector<Vector4> m_vecDirection;
	#pragma endregion

	protected:
		/* 인스턴스 모델 */
		std::shared_ptr<CInstanceEffectModel> m_spInstanceModel = nullptr;

		/* 인스턴스 메시 Info */
		FInstanceEffectMeshInfo m_tInstanceEffectMeshInfo = {};
	};
}

