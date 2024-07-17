#pragma once
#include "Model.h"

namespace Engine
{
	class CInstanceEffectMeshBuffer;
	class CBone;
	class CMaterial;
	class CAnimation;

	class ENGINE_DLL CInstanceEffectModel final : public CModel
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CInstanceEffectModel() = default;

		virtual ~CInstanceEffectModel() = default;

	/********************
		Events
	********************/
	public:
		static shared_ptr<CInstanceEffectModel> Create(const wstring& _wstrDatPathFile, Matrix _matPivot, const shared_ptr<CModel> _spParentModel, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo);

		HRESULT Initialize(const wstring& _wstrDatPathFile, Matrix _matPivot, const shared_ptr<CModel> _spParentModel, FInstanceEffectMeshInfo _tInstanceEffectMeshInfo);
		
		// HRESULT Render(uint32 _iMeshIndex);

	/********************
		Getter/Setter
	********************/
	public:
		ComPtr<ID3D11Buffer>& GetInstanceBuffer();

		vector<shared_ptr<CMeshBuffer>> m_vecMeshBuffers;
	};
}

