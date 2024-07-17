#pragma once
#include "Buffer.h"

#ifndef MAX_TEXTURE
#define MAX_TEXTURE 64
#endif

#ifndef MAX_INSTANCE_MESH
#define DEFAULT_INSTANCE_MESH 32
#endif

namespace Engine
{
	class CMeshHandler;
	class CBone;
	class CMaterial;
	class CEffectShader;

	class ENGINE_DLL CMeshBuffer : public CBuffer, public enable_shared_from_this<CMeshBuffer>
	{
	public:
		struct FMeshMatrix
		{
			Matrix matTransform;
			Matrix matParentWorld;
		};
		/********************
			Ctor/Dtor
		********************/
	public:
		CMeshBuffer() = default;
		virtual ~CMeshBuffer() = default;

		/********************
			Getter/Setter
		********************/
		EModelType GetModelType() { return m_eModelType; }
		EMeshManageType GetMeshType() { return m_eMeshType; }
		shared_ptr<CMaterial> GetMaterial() { return m_spMaterial; }
		wstring GetMeshName() { return m_wstrMeshName; }
		void SetMeshName(const wstring& _wstrMeshName) { m_wstrMeshName = _wstrMeshName; }

		EShaderType GetShaderType() { return m_eShaderType; }
		void SetShaderType(EShaderType _eShaderType) { m_eShaderType = _eShaderType; }

		int32 GetNumSRVs() { return (int32)m_vecTextureSRVs.size(); }
		ID3D11ShaderResourceView** GetRawSRVs() { return m_vecTextureSRVs.data(); }

		/********************
			Events
		********************/
	public:
		static shared_ptr<CMeshBuffer> Create(const wstring& _wstrModelFilePath, ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType, EMeshManageType _eDrawType);
		virtual HRESULT Initialize(const wstring& _wstrModelFilePath, ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType, EMeshManageType _eDrawType);
		virtual HRESULT BindBuffers_NonInstancing();
		virtual HRESULT BindBuffers_Instancing(ERenderType _eRenderType);
		void AddRenderInstance(const Matrix& _matTransform, const Matrix& _matParentWorld, ERenderType _eRenderType);
		void RefreshResources();							// Material ���� ����
		void RefreshInstances();	// Instance ���� ����
		virtual void Release() override;

		void SetMatrices(const Matrix& _matWorld, Matrix _matBones[256]);		// Unique �ִ�
		void SetMatrices(const Matrix& _matTransform, const Matrix& _matWorld);	// Unique ��ִ�
		void BindVariable(const ComPtr<ID3DX11EffectVariable>& _cpVariableMeshInfo, const ComPtr<ID3DX11EffectShaderResourceVariable>& _cpVariableTextures);

		virtual HRESULT Render() override;
		virtual HRESULT Render_Instance(ERenderType _eRenderType);

	protected:
		void InitVertexBuffer(ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType);
		HRESULT InitVertexBuffer_Anim(ifstream& _ifFile, Matrix _matPivot);
		HRESULT InitVertexBuffer_NonAnim(ifstream& _ifFile, Matrix _matPivot);
		HRESULT InitVertexBuffer_Landscape(ifstream& _ifFile, Matrix _matPivot);
		void InitIndexBuffer(ifstream& _ifFile);
		void InitInstanceBuffer();

	private:
		void RefreshResource_PlayerEye();
		void RefreshResource_MonsterFace();
		void RefreshResource_Default();
		void RefreshResource_PalLit_Top();
		void RefreshResource_Extern();
		void RefreshResource_Landscape();
		void RefreshResource_SkyDome();
		void RefreshResource_Architecture();	

		/********************
			Methods
		********************/
	public:
		void SetUpBoneMatrices(vector<Matrix>& _vecBoneMatrices, const vector<shared_ptr<CBone>>& _vecBones);

		/********************
			Data Members
		********************/
	protected: // Default
		wstring				                                                m_wstrMeshName = L"";
		EModelType			                                                m_eModelType = EModelType::ENUM_END;
		EMeshManageType			                                            m_eMeshType = EMeshManageType::ENUM_END;

		vector<wstring>		                                                m_vecBoneNames;                         // �޽��� ����ϴ� �� �̸���
		vector<uint32>		                                                m_vecBoneIndices;                       // �޽��� ����ϴ� �� �ε�����
		vector<Matrix>		                                                m_vecOffsetMatrices;

	protected:                                                                                                      // Instancing
		EShaderType															m_eShaderType = EShaderType::ENUM_END;	// �� �޽��� ����� �⺻ ���̴� �ڵ�
		any																	m_anyMeshInfo = nullptr;
		vector<ID3D11ShaderResourceView*>                                   m_vecTextureSRVs;						// �̸� ����� �� �⺻ ���׸��� �ؽ�ó �迭

		uint32                                                              m_iInstanceStride = 0;

		weak_ptr<CMeshHandler>												m_wpMeshHandler;			            // �޽� �ڵ鷯

		shared_ptr<CMaterial>                                               m_spMaterial = nullptr;					// �޽��� ����� �⺻ ���׸��� 

		unordered_map<ERenderType, ComPtr<ID3D11Buffer>>					m_umapInstanceBuffer;			        // �ν��Ͻ� ����
		unordered_map<ERenderType, vector<FMeshMatrix>>						m_umapInstanceMeshMatrix;				// ���̴� �� �ν��Ͻ� �� ���� ��ȯ ���
		unordered_map<ERenderType, uint32>									m_umapInstanceCurrentCount;				// ���̴� �� �ν��Ͻ� ���� ����
		unordered_map<ERenderType, uint32>									m_umapInstanceMaxCount;					// ���̴� �� �ν��Ͻ� �ִ� ����
		unordered_map<ERenderType, bool>									m_umapRegistered;						// ���� ��� ����
	};
}