#pragma once
#include "EffectComponent.h"

namespace Engine
{
	class CModel;
	class CMaterial;

	struct ENGINE_DLL FEffectMeshComponentDesc : public FEffectComponentDesc
	{
		std::wstring m_wstrJsonFileName = L""; // Json���� �ε� ���ϰ�, Ŭ�󿡼� ���� ����� ��� ����ֵ� �˴ϴ�.

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
#pragma region �ʱ�ȭ
	public:
		/* Json ���� �ε� */
		virtual HRESULT LoadFromJsonFile(const wstring& _wstrJsonFileName) override;

		/* Json ���� ���� */
		virtual HRESULT WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer) override;

	protected:
		/* ���� �ʱ�ȭ */
		virtual HRESULT InitializeBuffer() override;

		/* �ڵ鷯���� ����Ʈ ���̴� �������� */
		virtual HRESULT FindEffectShader() override;

		/* �ؽ�ó �������� */
		HRESULT FindShaderResourceViews();

#pragma region �Ϲ�
	public:
		/* ������Ʈ Ǯ���� ������ �� ����� �Լ� */
		virtual void RecycleFromPool() override;

		/* ��Ȱ��ȭ �� ���� */
		virtual void NoneActivate() override;

		/* ���� �� ���� */
		virtual void EndLifeTime() override;

		/* ���� �� ���� */
		virtual void Loop() override;

		/* ����Ʈ ���� */
		virtual void PlayEffect(float _fDeltaSeconds) override;

		/* Ʈ���� ���� */
		void PlayTrail(float _fDeltaSeconds);

		/* Ű������ ������ ���� */
		virtual void InterpolationBetweenKeyFrame(float _fDeltaSeconds) override;

		/* UVMove */
		virtual void UVMove(float _fDeltaSeconds) override;

		/* ��������Ʈ ��� */
		void TickSprite(float _fDeltaSeconds);

		/* Ű������ �߰� */
		void AddKeyFrame(FEffectMeshKeyFrame _tEffectMeshKeyFrmae);

		/********************
			Getter / Setter
		********************/
	public:
		/* Ű������ ���� */
		std::vector<FEffectMeshKeyFrame>& GetKeyFrames() { return m_tEffectMeshInfo.m_vecKeyFrames; }

		/* ����Ʈ �޽� Info */
		void SetEffectMeshInfo(const FEffectMeshInfo& _tEffectMeshInfo) { m_tEffectMeshInfo = _tEffectMeshInfo; }
		FEffectMeshInfo& GetEffectMeshInfo() { return m_tEffectMeshInfo; }

		/* ���� Ű������ �ε��� */
		void SetCurKeyFrameIndex(int32 _iCurKeyFrameIndex) { m_iCurKeyFrame = _iCurKeyFrameIndex; }
		uint32 GetCurKeyFrameIndex() { return m_iCurKeyFrame; }

		/* Ű������ */
		void SetKeyFrameValue(const FEffectMeshKeyFrame& _tEffectMeshKeyFrame, int32 _iIndex);
		const std::optional<FEffectMeshKeyFrame> GetKeyFrameValue(int32 _iIndex);

		void SetEffectShader(const std::wstring& _wstrEffectShaderName);
		void SetTexture(const std::wstring& _wstrTextureName, ETextureType _eTextureType);

		/********************
			Data Members
		********************/
	protected:
		/* Json ���� */
		std::wstring m_wstrJsonFileName = L"";

		/* �� ���� ��� */
		wstring m_wstrModelFilePath = L"";

		/* �ǹ� ��Ʈ����(����Ʈ�� �׵�) */
		Matrix m_matPivot = Matrix::Identity;

		/* �޽� Ʈ���� ���� */
		std::list<Matrix> m_lstMeshMatrices;

		float m_fTrailTimeAcc = 0.f;

	protected:
		/* �� */
		std::shared_ptr<CModel> m_spModel;

		/* Bind */
		FMeshInfo_Effect m_tMeshInfoEffect = {};

		/* Bind Variables */
		ComPtr<ID3DX11EffectMatrixVariable> m_cpVariable_Transform = nullptr;

		/* �ؽ�ó */
		vector<ID3D11ShaderResourceView*> m_vecSRVs;

		/* ���͸��� */// ���, ��Ż�� ���� ���� �߰� - �ؿ����� ��û -
		std::shared_ptr<CMaterial> m_spMaterial;

	protected:
		/* ����Ʈ �߻� �ٿ��� */
		Vector3 m_vOriginPos = Vector3::Zero;

		/* ����Ʈ �޽� Info */
		FEffectMeshInfo m_tEffectMeshInfo;

		/* ���� Ű������ �ε��� */
		int32 m_iCurKeyFrame = 0;

		/* ���� �Ϸ�� ������ */
		FEffectMeshKeyFrame m_tLerpedData;

		/* ��������Ʈ ���� ������ Ÿ�� */
		float m_fSpriteLifeTimeAcc = 0.f;

		/* ��������Ʈ UV */
		Vector2 m_vMinUV = Vector2::Zero;
		Vector2 m_vMaxUV = Vector2::One;
	};
}