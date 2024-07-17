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
#pragma region �ʱ�ȭ
	public:
		/* Json ���� �ε� */
		virtual HRESULT LoadFromJsonFile(const wstring& _wstrJsonFileName) override;

		/* Json ���� ���� */
		virtual HRESULT WriteToJsonFile(const wstring& _wstrJsonFileName, rapidjson::StringBuffer& _strBuffer, rapidjson::PrettyWriter<rapidjson::StringBuffer>& _writer) override;

	private:
		/* ���� �ʱ�ȭ */
		virtual HRESULT InitializeBuffer() override;

		/* �ν��Ͻ� �� �ʱ�ȭ */
		virtual void InitializeInstanceValue();

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
	#pragma region ����̽�
		/* ����̽� */
		ComPtr<ID3D11Device> m_cpDevice = nullptr;

		/* ���ؽ�Ʈ*/
		ComPtr<ID3D11DeviceContext> m_cpDeviceContext = nullptr;
	#pragma endregion

	protected:
	#pragma region �ν��Ͻ� ���
		/* ���ǵ� */
		std::vector<float> m_vecSpeed;

		/* ������ Ÿ�� */
		std::vector<float> m_vecLifeTime;

		/* ���� ������ Ÿ�� */
		std::vector<float> m_vecLifeTimeAcc;

		/* �����̼� */
		std::vector<Vector3> m_vecRotation;

		/* �ν��Ͻ� ������ ���� */
		std::vector<Vector4> m_vecDirection;
	#pragma endregion

	protected:
		/* �ν��Ͻ� �� */
		std::shared_ptr<CInstanceEffectModel> m_spInstanceModel = nullptr;

		/* �ν��Ͻ� �޽� Info */
		FInstanceEffectMeshInfo m_tInstanceEffectMeshInfo = {};
	};
}

