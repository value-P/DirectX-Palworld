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
		void RefreshResources();							// Material 정보 갱신
		void RefreshInstances();	// Instance 정보 갱신
		virtual void Release() override;

		void SetMatrices(const Matrix& _matWorld, Matrix _matBones[256]);		// Unique 애님
		void SetMatrices(const Matrix& _matTransform, const Matrix& _matWorld);	// Unique 논애님
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

		vector<wstring>		                                                m_vecBoneNames;                         // 메쉬가 사용하는 뼈 이름들
		vector<uint32>		                                                m_vecBoneIndices;                       // 메쉬가 사용하는 뼈 인덱스들
		vector<Matrix>		                                                m_vecOffsetMatrices;

	protected:                                                                                                      // Instancing
		EShaderType															m_eShaderType = EShaderType::ENUM_END;	// 이 메쉬가 사용할 기본 쉐이더 코드
		any																	m_anyMeshInfo = nullptr;
		vector<ID3D11ShaderResourceView*>                                   m_vecTextureSRVs;						// 미리 만들어 둔 기본 메테리얼 텍스처 배열

		uint32                                                              m_iInstanceStride = 0;

		weak_ptr<CMeshHandler>												m_wpMeshHandler;			            // 메쉬 핸들러

		shared_ptr<CMaterial>                                               m_spMaterial = nullptr;					// 메쉬가 사용할 기본 메테리얼 

		unordered_map<ERenderType, ComPtr<ID3D11Buffer>>					m_umapInstanceBuffer;			        // 인스턴스 버퍼
		unordered_map<ERenderType, vector<FMeshMatrix>>						m_umapInstanceMeshMatrix;				// 쉐이더 별 인스턴스 별 로컬 변환 행렬
		unordered_map<ERenderType, uint32>									m_umapInstanceCurrentCount;				// 쉐이더 별 인스턴스 현재 개수
		unordered_map<ERenderType, uint32>									m_umapInstanceMaxCount;					// 쉐이더 별 인스턴스 최대 개수
		unordered_map<ERenderType, bool>									m_umapRegistered;						// 렌더 등록 여부
	};
}