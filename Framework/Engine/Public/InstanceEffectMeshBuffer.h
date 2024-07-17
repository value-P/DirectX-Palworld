#pragma once
#include "MeshBuffer.h"

namespace Engine
{
	class ENGINE_DLL CInstanceEffectMeshBuffer final : public CMeshBuffer
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CInstanceEffectMeshBuffer() = default;

		virtual ~CInstanceEffectMeshBuffer() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CInstanceEffectMeshBuffer> Create(const wstring& _wstrModelFilePath, ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo);

		HRESULT Initialize(const wstring& _wstrModelFilePath, ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo);

		virtual HRESULT Render() override;

	protected:
		virtual HRESULT BindBuffers();

	/********************
		Methods
	********************/
	public:
		void InitializeNameBoneOffset(ifstream& _ifFile);

		void InitializeVertexBuffer(ifstream& _ifFile, Matrix _matPivot, EModelType _eModelType);

		void InitializeIndexBuffer(ifstream& _ifFile);

		void InitializeInstanceBuffer(FInstanceEffectMeshInfo _tInstanceEffectMeshInfo);

	/********************
		Data Members
	********************/
	public:
		ComPtr<ID3D11Buffer>& GetInstanceBuffer() { CHECKF(m_cpInstanceBuffer, L"Instance Buffer is nullptr : CInstanceEffectMeshBuffer"); return m_cpInstanceBuffer; }

	/********************
		Data Members
	********************/
	private:
		// 인스턴스 버퍼
		ComPtr<ID3D11Buffer> m_cpInstanceBuffer = nullptr;

		// 이펙트 인스턴스 메시
		FInstanceEffectMeshInfo m_tInstanceEffectMeshInfo = {};

	protected:
		uint32 m_iInstanceStride = 0;

		uint32 m_iIndexCountPerInstance = 0;
	};
}
