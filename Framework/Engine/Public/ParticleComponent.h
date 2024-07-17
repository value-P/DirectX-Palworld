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
		/* Json ���� �ε� */
		virtual HRESULT LoadFromJsonFile(const wstring& _wstrJsonFileName) override;

		/* Json ���� ���� */
		virtual HRESULT WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer) override;

	private:
		/* ���� �ʱ�ȭ */
		virtual HRESULT InitializeBuffer() override;

		/* �ڵ鷯���� ����Ʈ ���̴� �������� */
		virtual HRESULT FindEffectShader() override;

		/* �ؽ�ó �������� */
		HRESULT FindShaderResourceViews();

		/* �ν��Ͻ� �� �ʱ�ȭ */
		virtual void InitializeInstanceValue();

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

		/* Ű������ ������ ���� */
		virtual void InterpolationBetweenKeyFrame(float _fDeltaSeconds) override;

		/* UVMove */
		virtual void UVMove(float _fDeltaSeconds) override;

		/* Ű������ �߰� */
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
		/* Ű������ ���� */
		std::vector<FParticleKeyFrame>& GetKeyFrames() { return m_tParticleInfo.m_vecKeyFrames; }

		/* �θ� ������Ʈ ��ġ ��ȯ */
		Vector3 GetOwnerPositionV3();
		Vector4 GetOwnerPosition();

		/* �θ� ������Ʈ ��� ��ȯ */
		Matrix GetOwnerMatrix();

		/* ��ƼŬ Info */
		void SetParticleInfo(const FParticleInfo& _tParticleInfo) { m_tParticleInfo = _tParticleInfo; }
		FParticleInfo& GetParticleInfo() { return m_tParticleInfo; }

		/* ���� Ű������ �ε��� */
		void SetCurKeyFrameIndex(int32 _iCurKeyFrameIndex) { m_iCurKeyFrame = _iCurKeyFrameIndex; }
		uint32 GetCurKeyFrameIndex() { return m_iCurKeyFrame; }

		/* Ű������ */
		void SetKeyFrameValue(const FParticleKeyFrame& _tParticleKeyFrame, int32 _iIndex);
		const std::optional<FParticleKeyFrame> GetKeyFrameValue(int32 _iIndex);

		void SetEffectShader(const std::wstring& _wstrEffectShaderName);
		void SetTexture(const std::wstring& _wstrTextureName, ETextureType _eTextureType);

	/********************
		Data Members
	********************/
	protected:
	#pragma region �� ��ü
		/* ����̽� */
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		/* ���ؽ�Ʈ*/
		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;

		/* �ν��Ͻ� ���� */
		std::shared_ptr<CInstancing> m_spInstancing;

		/* �ؽ�ó */
		vector<ID3D11ShaderResourceView*> m_vecSRVs;

		/* Bind */
		FEffectInfo_Particle m_tEffectInfoParticle = {};

		/* ������ �ؽ�ó */
		ComPtr<ID3D11ShaderResourceView> m_cpDissolveTexture = nullptr;
#pragma endregion

	protected:
	#pragma region �ν��Ͻ� ���
		/* ���ǵ� */
		std::vector<float> m_vecSpeed;

		/* ������ Ÿ�� */
		std::vector<float> m_vecLifeTime;

		/* ���� ������ Ÿ�� */
		std::vector<float> m_vecLifeTimeAcc;

		/* ��������Ʈ ���� ������ Ÿ�� */
		std::vector<float> m_vecSpriteLifeTimeAcc;

		/* ���� ���� ���� �� */
		std::vector<float> m_vecAlphaDecreaseAcc;

		/* �����̼� */
		std::vector<Vector3> m_vecRotation;

		/* �ν��Ͻ� ������ ���� */
		std::vector<Vector4> m_vecDirection;
#pragma endregion

	protected:
	#pragma region ��ƼŬ Base
		/* ����Ʈ �߻� �ٿ��� */
		Vector3 m_vOriginPos = Vector3::Zero;

		/* ��ƼŬ Info */
		FParticleInfo m_tParticleInfo;

		/* ���� Ű������ �ε��� */
		int32 m_iCurKeyFrame = 0;

		/* ���� �Ϸ�� ������ */
		FParticleKeyFrame m_tLerpedData;
#pragma endregion
	};
}

