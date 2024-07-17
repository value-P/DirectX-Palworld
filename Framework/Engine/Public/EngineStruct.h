#pragma once

namespace Engine
{
	/********************
		Input Layout
	********************/
	struct ENGINE_DLL FPosition final
	{
		XMFLOAT3 m_vPosition;

		const static UINT iNumInputElementDescs = 1;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[1];
	};

	struct ENGINE_DLL FPositionSize final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT2 m_vPSize;

		const static UINT iNumInputElementDescs = 2;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[2];
	};

	struct ENGINE_DLL FPositionColor final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT3 m_vColor;

		const static UINT iNumInputElementDescs = 2;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[2];
	};

	struct ENGINE_DLL FPositionSizeColor final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT2 m_vPSize;
		XMFLOAT3 m_vColor;

		const static UINT iNumInputElementDescs = 3;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[3];
	};

	struct ENGINE_DLL FPoint final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT2 m_vPSize;

		const static UINT iNumInputElementDescs = 2;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[2];
	};

	struct ENGINE_DLL FPositionTexCoord2 final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT2 m_vTexCoord2;

		const static UINT iNumInputElementDescs = 2;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[2];
	};

	struct ENGINE_DLL FPositionTexCoord2Size final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT2 m_vTexCoord2;
		XMFLOAT2 m_vSize;

		const static UINT iNumInputElementDescs = 3;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[3];
	};

	struct ENGINE_DLL FPositionTexCoord3 final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT3 m_vTexCoord3;

		const static UINT iNumInputElementDescs = 2;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[2];
	};

	struct ENGINE_DLL FPositionColorTexCoord2 final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT3 m_vColor;
		XMFLOAT2 m_vTexCoord2;

		const static UINT iNumInputElementDescs = 3;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[3];
	};

	struct ENGINE_DLL FPositionNormalTexCoord2 final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT3 m_vNormal;
		XMFLOAT2 m_vTexCoord2;

		const static UINT iNumInputElementDescs = 3;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[3];
	};

	struct ENGINE_DLL FPositionNormalTexCoord2Tangent final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT3 m_vNormal;
		XMFLOAT2 m_vTexCoord2;
		XMFLOAT3 m_vTangent;

		const static UINT iNumInputElementDescs = 4;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[4];
	};

	struct ENGINE_DLL FPositionNormalTexCoord2TexCoord2Tangent final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT3 m_vNormal;
		XMFLOAT2 m_vTexCoord21;
		XMFLOAT2 m_vTexCoord22;
		XMFLOAT3 m_vTangent;

		const static UINT iNumInputElementDescs = 5;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[5];
	};

	struct ENGINE_DLL FPositionNormalTexCoord2TangentBlendIndexBlendWeight final
	{
		XMFLOAT3 m_vPosition;
		XMFLOAT3 m_vNormal;
		XMFLOAT2 m_vTexCoord2;
		XMFLOAT3 m_vTangent;
		XMUINT4 m_vBlendIndices;
		XMFLOAT4 m_vBlendWeights;

		const static UINT iNumInputElementDescs = 6;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[6];
	};

	struct ENGINE_DLL FTransformParentWorld final
	{
		XMFLOAT4 m_vTransformRight;
		XMFLOAT4 m_vTransformUp;
		XMFLOAT4 m_vTransformLook;
		XMFLOAT4 m_vTransformPosition;

		XMFLOAT4 m_vParentWorldRight;
		XMFLOAT4 m_vParentWorldUp;
		XMFLOAT4 m_vParentWorldLook;
		XMFLOAT4 m_vParentWorldPosition;
	};

	struct ENGINE_DLL FNonAnimMeshInstance final
	{
		const static UINT iNumInputElementDescs = 12;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[12];
	};

	struct ENGINE_DLL FRightUpLookPositionPrevPosition final
	{
		XMFLOAT4 m_vRight;
		XMFLOAT4 m_vUp;
		XMFLOAT4 m_vLook;
		XMFLOAT4 m_vTranslation;
		XMFLOAT4 m_vPrevTranslation;
	};

	struct ENGINE_DLL FEffectMeshInstance final
	{
		const static UINT iNumInputElementDescs = 9;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[9];
	};

	struct ENGINE_DLL FRightUpLookPositionIndexIsDead final
	{
		XMFLOAT4 m_vRight;
		XMFLOAT4 m_vUp;
		XMFLOAT4 m_vLook;
		XMFLOAT4 m_vTranslation;
		XMFLOAT2 m_vIndexAndIsDead; // x : �ν��Ͻ� �ĺ���, y : ��� ����
	};

	struct ENGINE_DLL FMeshInstance final
	{
		const static UINT iNumInputElementDescs = 9;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[9];
	};

	struct ENGINE_DLL FRightUpLookPositionColor final
	{
		XMFLOAT4 m_vRight;
		XMFLOAT4 m_vUp;
		XMFLOAT4 m_vLook;
		XMFLOAT4 m_vTranslation;
		XMFLOAT4 m_vPrevTranslation;

		XMFLOAT4 m_vColor;

		XMFLOAT2 m_vMinUV;
		XMFLOAT2 m_vMaxUV;
	};

	struct ENGINE_DLL FPointInstance final
	{
		const static UINT iNumInputElementDescs = 10;
		const static D3D11_INPUT_ELEMENT_DESC m_tInputElementDescs[10];
	};

	/********************
		Cbuffer
	********************/
	// Matrix
	struct FConstantBuffer { };

	struct FOneMatrixConstantBuffer final : public FConstantBuffer
	{
		Matrix m_matMatrix_0 = Matrix::Identity;
	};

	struct FTwoMatrixConstantBuffer final : public FConstantBuffer
	{
		Matrix m_matMatrix_0 = Matrix::Identity;
		Matrix m_matMatrix_1 = Matrix::Identity;
	};

	struct FThreeMatrixConstantBuffer final : public FConstantBuffer
	{
		Matrix m_matMatrix_0 = Matrix::Identity;
		Matrix m_matMatrix_1 = Matrix::Identity;
		Matrix m_matMatrix_2 = Matrix::Identity;
	};

	struct FFourMatrixConstantBuffer final : public FConstantBuffer
	{
		Matrix m_matMatrix_0 = Matrix::Identity;
		Matrix m_matMatrix_1 = Matrix::Identity;
		Matrix m_matMatrix_2 = Matrix::Identity;
		Matrix m_matMatrix_3 = Matrix::Identity;
	};

	struct FMatrix256ConstantBuffer final : public FConstantBuffer
	{
		Matrix m_matMatrix[256]{ Matrix::Identity };
	};


	// Int
	struct FOneIntConstantBuffer final : public FConstantBuffer
	{
		int32 m_iInt_0 = 0;

	private:
		int32 m_iDummies[3]{ 0 };
	};

	struct FTwoIntConstantBuffer final : public FConstantBuffer
	{
		int32 m_iInt_0 = 0;
		int32 m_iInt_1 = 0;

	private:
		int32 m_iDummies[2]{ 0 };
	};

	struct FThreeIntConstantBuffer final : public FConstantBuffer
	{
		int32 m_iInt_0 = 0;
		int32 m_iInt_1 = 0;
		int32 m_iInt_2 = 0;

	private:
		int32 m_iDummies[1]{ 0 };
	};

	struct FFourIntConstantBuffer final : public FConstantBuffer
	{
		int32 m_iInt_0 = 0;
		int32 m_iInt_1 = 0;
		int32 m_iInt_2 = 0;
		int32 m_iInt_3 = 0;
	};


	// Float
	struct FOneFloatConstantBuffer final : public FConstantBuffer
	{
		float m_fFloat_0 = 0.0f;

	private:
		float m_fDummies[3]{ 0.0f };
	};

	struct FTwoFloatConstantBuffer final : public FConstantBuffer
	{
		float m_fFloat_0 = 0.0f;
		float m_fFloat_1 = 0.0f;

	private:
		float m_fDummies[2]{ 0.0f };
	};

	struct FThreeFloatConstantBuffer final : public FConstantBuffer
	{
		float m_fFloat_0 = 0.0f;
		float m_fFloat_1 = 0.0f;
		float m_fFloat_2 = 0.0f;

	private:
		float m_fDummies[1]{ 0.0f };
	};

	struct FFourFloatConstantBuffer final : public FConstantBuffer
	{
		float m_fFloat_0 = 0.0f;
		float m_fFloat_1 = 0.0f;
		float m_fFloat_2 = 0.0f;
		float m_fFloat_3 = 0.0f;
	};


	// Vector3
	struct FOneVector3ConstantBuffer final : public FConstantBuffer
	{
		Vector3 m_vVector3_0 = Vector3::Zero;

	private:
		float m_fDummies[1]{ 0.0f };
	};

	struct FTwoVector3ConstantBuffer final : public FConstantBuffer
	{
		Vector3 m_vVector3_0 = Vector3::Zero;
		Vector3 m_vVector3_1 = Vector3::Zero;

	private:
		float m_fDummies[2]{ 0.0f };
	};

	struct FThreeVector3ConstantBuffer final : public FConstantBuffer
	{
		Vector3 m_vVector3_0 = Vector3::Zero;
		Vector3 m_vVector3_1 = Vector3::Zero;
		Vector3 m_vVector3_2 = Vector3::Zero;

	private:
		float m_fDummies[3]{ 0.0f };
	};

	struct FFourVector3ConstantBuffer final : public FConstantBuffer
	{
		Vector3 m_vVector3_0 = Vector3::Zero;
		Vector3 m_vVector3_1 = Vector3::Zero;
		Vector3 m_vVector3_2 = Vector3::Zero;
		Vector3 m_vVector3_3 = Vector3::Zero;
	};


	// Vector4
	struct FOneVector4ConstantBuffer final : public FConstantBuffer
	{
		Vector4 m_vVector4_0 = Vector4::Zero;
	};

	struct FTwoVector4ConstantBuffer final : public FConstantBuffer
	{
		Vector4 m_vVector4_0 = Vector4::Zero;
		Vector4 m_vVector4_1 = Vector4::Zero;
	};

	struct FThreeVector4ConstantBuffer final : public FConstantBuffer
	{
		Vector4 m_vVector4_0 = Vector4::Zero;
		Vector4 m_vVector4_1 = Vector4::Zero;
		Vector4 m_vVector4_2 = Vector4::Zero;
	};

	struct FFourVector4ConstantBuffer final : public FConstantBuffer
	{
		Vector4 m_vVector4_0 = Vector4::Zero;
		Vector4 m_vVector4_1 = Vector4::Zero;
		Vector4 m_vVector4_2 = Vector4::Zero;
		Vector4 m_vVector4_3 = Vector4::Zero;
	};

	/********************
		Effect
	********************/
	/* Particle */
	struct ENGINE_DLL FParticleKeyFrame final
	{
		/* ����Ʈ ���̴� �н� �ε��� */
		int32 m_iPassIndex = 0;

		/* Ű������ �ð� */
		float m_fTime = 0.f;

		/* ���� �� */
		bool m_bRenderCulled = false;

		/* ũ�� ���� */
		float		m_fSizeRatio = 1.f;
		EEasingType m_eSizeEasingType = EEasingType::EASE_LINEAR;
		float		m_fSizeEasingExpo = 2.f;

		/* �ӵ� ���� */
		float		m_fSpeedRatio = 1.f;
		EEasingType m_eSpeedEasingType = EEasingType::EASE_LINEAR;
		float		m_fSpeedEasingExpo = 2.f;

		/* ��ǻ�� �÷�1 */
		Vector4		m_vColor1 = Vector4::One;
		float		m_fColor1Weight = 0.3333f;
		EEasingType m_eColor1EasingType = EEasingType::EASE_LINEAR;
		float		m_fColor1EasingExpo = 2.f; // ���� ���� ��� ����Ѵٸ�, �� �������� �Է����־�� �մϴ�. ����Ʈ : 2�� �Լ�

		/* ��ǻ�� �÷�2 */
		Vector4		m_vColor2 = Vector4::One;
		float		m_fColor2Weight = 0.3333f;
		EEasingType m_eColor2EasingType = EEasingType::EASE_LINEAR;
		float		m_fColor2EasingExpo = 2.f;

		/* ��ǻ�� �÷�3 */
		Vector4		m_vColor3 = Vector4::One;
		float		m_fColor3Weight = 0.3333f;
		EEasingType m_eColor3EasingType = EEasingType::EASE_LINEAR;
		float		m_fColor3EasingExpo = 2.f;
	};

	struct ENGINE_DLL FParticleInfo final
	{
		/* ���� Ÿ�� */
		ERenderType m_eRenderType = ERenderType::ENUM_END;

		/* ����Ʈ Ÿ�� */
		EEffectType m_eEffectType = EEffectType::PARTICLE;

		/* ����Ʈ ���̴� �̸� */
		std::wstring m_wstrEffectShaderName = L"";

		/* ��ƼŬ Ÿ�� */
		EParticleType m_eParticleType = EParticleType::ENUM_END;

		/* ��ǻ�� �ؽ�ó */
		bool m_bDiffuse = false;
		std::wstring m_wstrDiffuseTextureName = L"";

		/* ����ũ �ؽ�ó */
		bool m_bMask = false;
		std::wstring m_wstrMaskTextureName = L"";

		/* ��������Ʈ ��� */
		bool m_bSprite = false;
		int32 m_iSpriteRow = 1;
		int32 m_iSpriteCol = 1;
		float m_fSpriteSpeed = 1.f;

		/* ���� */
		bool m_bLoop = false;

		/* ������ */
		bool m_bBillboard = false;

		/* ���� ��� */
		bool m_bDirectional = false;

		/* �߷� */
		bool m_bGravity = false;
		float m_fGravityValue = 9.8f;

		/* �۷ο� */
		bool m_bGlow = false;
		float m_fGlowWeight = 0.0f;

		/* ��� */
		bool m_bBloom = false;
		float m_fBloomWeight = 0.0f;

		/* ���� ���� */
		bool m_bAlphaAttenuation = false;
		float m_fAlphaAttenuationWeight = 0.0f;

		/* ���� ������ Ÿ�� ��� */
		bool m_bRandomLifeTime = true;

		/* ������ Ÿ�� */
		Vector2 m_vLifeTime = { 0.0f, 1.0f };

		/* �ν��Ͻ� ���� */
		int32 m_iNumInstance = 1;

		/* ũ�� */
		Vector2 m_vSize = Vector2::One;

		/* �÷� */
		Vector4 m_vColor1 = Vector4::One;
		Vector4 m_vColor2 = Vector4::One;
		Vector4 m_vColor3 = Vector4::One;

		/* ���� �ӵ� */
		Vector2 m_vSpeed = Vector2::Zero;

		/* ��ü�κ��� ������ ��ġ */
		Vector3 m_vOffset = Vector3::Zero;

		/* �ǹ� */
		Vector3 m_vPivot = Vector3::Zero;

		/* ���� ���� */
		Vector3 m_vRange = Vector3::Zero;

		/* �ʴ� ȸ�� ���� */
		Vector3 m_vRotation = Vector3::Zero;

		/* �ּ�, �ִ� �����ݰ� */
		Vector2 m_vRadius = Vector2::One;

		/* ��ƼŬ Ű������ �� */
		uint16 m_iNumKeyFrames = 0;

		/* ��ƼŬ Ű������ */
		std::vector<FParticleKeyFrame> m_vecKeyFrames;
	};

	/* EffectMesh */
	struct ENGINE_DLL FEffectMeshKeyFrame final
	{
		/* ����Ʈ ���̴� �н� �ε��� */
		int32 m_iPassIndex = 0;

		/* Ű������ �ð� */
		float m_fTime = 0.f;

		/* ���� �� */
		bool m_bRenderCulled = false;

		/* ��ǻ�� �÷�1 */
		Vector4		m_vColor1 = Vector4::One;
		float		m_fColor1Weight = 0.3333f;
		EEasingType m_eColor1EasingType = EEasingType::EASE_LINEAR;
		float		m_fColor1EasingExpo = 2.f;

		/* ��ǻ�� �÷�2 */
		Vector4		m_vColor2 = Vector4::One;
		float		m_fColor2Weight = 0.3333f;
		EEasingType m_eColor2EasingType = EEasingType::EASE_LINEAR;
		float		m_fColor2EasingExpo = 2.f;

		/* ��ǻ�� �÷�3 */
		Vector4		m_vColor3 = Vector4::One;
		float		m_fColor3Weight = 0.3333f;
		EEasingType m_eColor3EasingType = EEasingType::EASE_LINEAR;
		float		m_fColor3EasingExpo = 2.f;

		/* Scale */
		Vector3		m_vScale = Vector3::One;
		EEasingType m_eScaleEasingType = EEasingType::EASE_LINEAR;
		float		m_fScaleEasingExpo = 2.f;

		/* Rotation */
		Vector3		m_vRotation = Vector3::Zero;
		EEasingType m_eRotationEasingType = EEasingType::EASE_LINEAR;
		float		m_fRotationEasingExpo = 2.f;
		float		m_fRotateWeight = 0.f; // ȸ�� ����ġ

		/* Translation */
		Vector3		m_vTranslation = Vector3::Zero;
		EEasingType m_eTranslationEasingType = EEasingType::EASE_LINEAR;
		float		m_fTranslationEasingExpo = 2.f;
	};

	struct ENGINE_DLL FEffectMeshInfo final
	{
		/* ���� Ÿ�� */
		ERenderType m_eRenderType = ERenderType::ENUM_END;

		/* ����Ʈ Ÿ�� */
		EEffectType m_eEffectType = EEffectType::EFFECTMESH;

		/* �� ���� ��� */
		std::wstring m_wstrModelFilePath = L"";

		/* ����Ʈ ���̴� �̸� */
		std::wstring m_wstrEffectShaderName = L"";

		/* �޽� Ʈ���� ���� */
		bool m_bTrail = false;

		/* �޽� Ʈ���� ���� */
		int32 m_iNumTrails = 1;

		/* �޽� Ʈ���� �ð� ���� */
		float m_fTrailTerm = 0.05f;

		/* �ʱ� S R T */
		Vector3 m_vScale = Vector3::One;
		Vector3 m_vRotation = Vector3::Zero;
		Vector3 m_vTranslation = Vector3::Zero;

		/* �ִ� ���� */
		bool m_bAnimation = false;

		/* ���� ���� */
		bool m_bLoop = false;

		/* ��ǻ�� �ؽ�ó */
		bool m_bDiffuse = false;
		std::wstring m_wstrDiffuseTextureName = L"";

		/* ��ǻ�� �÷� */
		Vector4 m_vDiffuseColor1 = Vector4::One;
		Vector4 m_vDiffuseColor2 = Vector4::One;
		Vector4 m_vDiffuseColor3 = Vector4::One;

		/* ������ �ؽ�ó */
		bool m_bNoise = false;
		std::wstring m_wstrNoiseTextureName = L"";

		/* ����ũ �ؽ�ó */
		bool m_bMask = false;
		std::wstring m_wstrMaskTextureName = L"";

		/* ����� */
		bool m_bDistortion = false;
		std::wstring m_wstrDistortionTextureName = L"";
		float m_fDistortionWeight = 0.f;

		/* ������ */
		bool m_bDissolve = false;
		std::wstring m_wstrDissolveTextureName = L"";
		float m_fDissolveWeight = 0.f;

		/* ������ Ÿ�� */
		Vector2 m_vLifeTime = { 0.f, 1.f };

		/* UVMove */
		bool	m_bUVMoveLoop = false;
		Vector2	m_vUVMoveSpeed = { 0.f, 0.f }; // -> {0, 0}���� �θ� UV�� �������� �ʽ��ϴ�.

		/* ��������Ʈ ��� */
		bool m_bSprite = false;
		int32 m_iSpriteRow = 1;
		int32 m_iSpriteCol = 1;
		float m_fSpriteSpeed = 1.f;

		/* �۷ο� */
		bool m_bGlow = false;
		float m_fGlowWeight = 0.f;

		/* ��� */
		bool m_bBloom = false;
		float m_fBloomWeight = 0.f;

		/* ���� ���� */
		bool m_bAlphaAttenuation = false;
		float m_fAlphaAttenuationWeight = 0.f;

		/* ���� �ڸ��� ���� �� */
		float m_fAlphaDiscardValue = 0.5f;

		/* ����Ʈ �޽� Ű������ �� */
		uint16 m_iNumKeyFrames = 0;

		/* ����Ʈ �޽� Ű������ */
		std::vector<FEffectMeshKeyFrame> m_vecKeyFrames;
	};

	struct ENGINE_DLL FInstanceEffectMeshInfo final
	{
		/* ��ƼŬ Ÿ�� */
		EParticleType m_eParticleType = EParticleType::ENUM_END;

		int32	m_iNumInstance = 0;

		bool	m_bGravity = false;
		float	m_fGravityValue = 9.8f;

		Vector2 m_vSize = Vector2::One;
		Vector2 m_vSpeed = Vector2::Zero;
		Vector3 m_vRange = Vector3::Zero;
		Vector2 m_vRadius = Vector2::One;
		Vector3 m_vRotation = Vector3::Zero;
	};

	// �ִϸ��̼� �̺�Ʈ

	// �浹 �̺�Ʈ ������
	struct ENGINE_DLL FCollisionNotifyData
	{
		int m_iColliderCount = 0;
		vector<float> m_vecColliderRadius;
		vector<Vector3> m_vecColliderRelativeLocation;
		int m_iAttachBoneIndex = 0;
	};

	// ���� �̺�Ʈ ������
	struct ENGINE_DLL FSoundNotifyData
	{
		wstring m_wstrSoundID = L"";
	};

	// ����Ʈ �̺�Ʈ ������
	struct ENGINE_DLL FEffectNotifyData
	{
		bool m_bUseFollowBone = false;
		int  m_iAttachBoneIndex = 0;
		wstring m_wstrEffectName = L"";
		Vector3 m_vRelativeLocation = Vector3(0.f, 0.f, 0.f);
	};

	struct ENGINE_DLL FAnimNotifyData
	{
		// ������ ����
		ENotifyType m_eNotifyType = ENotifyType::ENUM_END;
		float m_fStartTime = 0.f;
		float m_fEndTime = 0.f;
		float m_fDuration = 0.f;

		FCollisionNotifyData m_tCollisionNotify;

		FSoundNotifyData m_tSoundNotify;

		FEffectNotifyData m_tEffectNotify;

		// ������ �Լ� �̸�
		wstring m_wstrFunctionName = L"";
	};

	struct ENGINE_DLL FAnimationData
	{
		wstring m_wstrCurrentAnimName = L"";

		int m_iCurrentAnimID = -1;
		int m_iPrevAnimID = -1;
		int m_iNextAnimID = -1;

		int m_iLoopCount = 0;

		float m_fCurrentKeyFrameRate = 0.f;
		float m_fTrackPosition = 0.f;
		float m_fDuration = 0.f;

		bool m_bLoopEnd = false;
		bool m_bIsLerping = false;
	};

	struct ENGINE_DLL FModelAnimData
	{
		Vector3 m_vRootmotionVelocity = Vector3(0.f, 0.f, 0.f);

		// �⺻�� �Ǵ� �ִϸ��̼� ����
		FAnimationData m_tBaseAnimData;

		// ����ü �и� �� ��ü�� �ִϸ��̼� ����
		FAnimationData m_tBlendAnimData;
	};

	// Light
	struct ENGINE_DLL FDirectionalLight
	{
		Vector3 vDirection; // ������ ����, ����: ����, ����: -1.0 ~ 1.0 (����ȭ�� ����)
		float fIntensity; // ������ ����, ����: ����, ����: 0.0 ~ ���Ѵ� (�Ϲ������� 0.0 ~ 1.0)

		Vector3 vColor; // ������ ����, ����: ����, ����: 0.0 ~ 1.0 (RGB ���� ����)
		float pad1;
	};

	struct ENGINE_DLL FPointLight
	{
		Vector3 vPosition; // ������ ��ġ, ����: ���� ���� (��: ����), ����: �Ǽ� ��ü
		float fIntensity; // ������ ����, ����: ����, ����: 0.0 ~ ���Ѵ� (�Ϲ������� 0.0 ~ 1.0)

		Vector3 vColor; // ������ ����, ����: ����, ����: 0.0 ~ 1.0 (RGB ���� ����)
		float fRadius; // ������ �ݰ�, ����: ���� ���� (��: ����), ����: 0.0 ~ ���Ѵ�

		float fFalloff; // ������ ������, ����: ����, ����: 0.0 ~ ���Ѵ� (�Ϲ������� 0.0 ~ 1.0)
		Vector3 pad1;
	};

	struct ENGINE_DLL FAmbientLight
	{
		Vector3 vColor = Vector3(0.1f, 0.1f, 0.1f); // ������ ����, ����: ����, ����: 0.0 ~ 1.0 (RGB ���� ����)
		float fIntensity = 0.5f;// ������ ����, ����: ����, ����: 0.0 ~ ���Ѵ� (�Ϲ������� 0.0 ~ 1.0)
	};

	struct ENGINE_DLL FLightInfo
	{
		FDirectionalLight tDirectionalLight;
		FPointLight tPointLights[8];
		uint32 iPointLightCount = 0;
		FAmbientLight tAmbientLight;
	};

	// Cbuffer ��� //

	// ������ ���� ����
	struct ENGINE_DLL FFrameInfo final
	{
		float fDeltaTime = 0;
		float fTotalTime = 0;
		int32 iDebugRenderTarget = 2;
		int32 iFramePad1 = 0;
	};

	// ī�޶� ���� ���� 
	struct ENGINE_DLL FCameraInfo final
	{
		Matrix matView;

		Matrix matProj;

		Matrix matViewProj;

		Vector3 vCameraPosition;
		int32 iCameraPad1;
	};

	// �޽� - �÷��̾� ��
	struct ENGINE_DLL FMeshInfo_PlayerEye final
	{
		Matrix matBones[256];

		Matrix matParentWorld;

		int32 iCorneaIndex = 0;
		Vector3 vCorneaAlternativeColor = Vector3::Zero;

		int32 iHighlightIndex = 0;
		Vector3 vHighlightAlternativeColor = Vector3::Zero;

		int32 iPupilIndex = 0;
		Vector3 vPupliAlternativeColor = Vector3::Zero;

		int32 bCorneaUseAlternativeColor = false;
		int32 bHighlightUseAlternativeColor = false;
		int32 bPupilUseAlternativeColor = false;
		int32 bPad1 = 0;

		float fCorneaEmission = 0;
		float fHighlightEmission = 0;
		float fPupliEmission = 0;
		float fMeshPlayerEyePad1 = 0;
	};

	// �޽� - ���� ��, ��
	struct ENGINE_DLL FMeshInfo_MonsterFace final
	{
		Matrix matBones[256];

		Matrix matParentWorld;

		int32 iMaxIndexX = 0;
		int32 iIndexX = 0;
		int32 iMaxIndexY = 0;
		int32 iIndexY = 0;
	};

	// �޽� - �⺻ �ִ�
	struct ENGINE_DLL FMeshInfo_Default_Anim final
	{
		Matrix matBones[256];

		Matrix matParentWorld;

		int32 bDiffuse = false;
		int32 bNormal = false;
		int32 bMRSO = false;
		int32 bEmission = false;

		int32 bUseAlternativeColor = false;
		Vector3 vAlternativeColor = Vector3::Zero;

		float fRoughnessAdd = 0;
		float fOcclusionAdd = 0;
		Vector2 vPad1;

		float fDeltaSeconds = 0.001f;
	};

	// �޽� - �⺻ ��ִ�
	struct ENGINE_DLL FMeshInfo_Default_NonAnim final
	{
		Matrix matTransform;

		Matrix matParentWorld;

		int32 bDiffuse = false;
		int32 bNormal = false;
		int32 bMRSO = false;
		int32 bEmission = false;

		int32 bUseAlternativeColor = false;
		Vector3 vAlternativeColor = Vector3::Zero;

		float fRoughnessAdd = 0;
		float fOcclusionAdd = 0;
		Vector2 vPad1;
	};

	// �޽� - �⺻ ��ִ� �ν��Ͻ�
	struct ENGINE_DLL FMeshInfo_Default_Instance final
	{
		int32 bDiffuse = false;
		int32 bNormal = false;
		int32 bMRSO = false;
		int32 bEmission = false;

		int32 bUseAlternativeColor = false;
		Vector3 vAlternativeColor = Vector3::Zero;

		float fRoughnessAdd = 0;
		float fOcclusionAdd = 0;
		Vector2 vPad1;
	};

	// �޽� - Pal Top NonAnim
	struct ENGINE_DLL FMeshInfo_PalLitTop_NonAnim final
	{
		Matrix matTransform;

		Matrix matParentWorld;

		int32 bDiffuse = false;
		int32 bNormal = false;
		int32 bMRSO = false;
		int32 bEmission = false;

		int32 bTopDiffuse = false;
		int32 bTopNormal = false;
		float fRoughnessAdd = 0;
		float fOcclusionAdd = 0;
	};

	// �޽� - Pal Top �ν��Ͻ�
	struct ENGINE_DLL FMeshInfo_PalLitTop_Instance final
	{
		int32 bDiffuse = false;
		int32 bNormal = false;
		int32 bMRSO = false;
		int32 bEmission = false;

		int32 bTopDiffuse = false;
		int32 bTopNormal = false;
		float fRoughnessAdd = 0;
		float fOcclusionAdd = 0;
	};

	// �޽� - Extern NonAnim
	struct ENGINE_DLL FMeshInfo_Extern_NonAnim final
	{
		Matrix matTransform;

		Matrix matParentWorld;

		int32 bDiffuse = false;
		int32 bNormal = false;
		int32 bSpecularInvRoughness = false;
		int32 bExternPad1 = 0;
	};

	// �޽� - Extern �ν��Ͻ�
	struct ENGINE_DLL FMeshInfo_Extern_Instance final
	{
		int32 bDiffuse = false;
		int32 bNormal = false;
		int32 bSpecularInvRoughness = false;
		int32 bExternPad1 = 0;
	};

	// �޽� - ����
	struct ENGINE_DLL FMeshInfo_Landscape final
	{
		Matrix matCombindWorld;

		// ���γ��� ���̾�׷� ĭ ������
		float fVoronoiCellSizeX = 20;
		float fVoronoiCellSizeY = 20;

		// ���γ��� ���̾�׷��� ĭ �ϳ��� �ݺ� �� �⺻ �ؽ�ó ��
		float fNumRepeatX = 1;
		float fNumRepeatY = 1;
	};

	// �޽� - �ϴ�
	struct ENGINE_DLL FMeshInfo_SkyDome final
	{
		Matrix matTransform;

		Matrix matParentWorld;
	};

	// �޽� - ���๰ - ��ġ
	struct ENGINE_DLL FMeshInfo_Architecture final
	{
		Matrix matTransform;

		Matrix matParentWorld;

		int32 bDiffuse = false;
		int32 bNormal = false;
		int32 bMRSO = false;
		int32 bEmission = false;

		float fRoughnessAdd = 0;
		float fOcclusionAdd = 0;
		int32 iState = (int32)EArchitextureState::ENUM_END;
		float fAlpha = 0.5f;

		float fExtendY = 0;
		float fProgress = 0;
		Vector2 Pad1;

		Vector3 vCenter;
		float Pad2 = 0;
	};

	// �޽� - ����Ʈ
	struct ENGINE_DLL FMeshInfo_Effect final
	{
		Matrix matGroupWorld;

		Matrix matWorld;

		int32 bDiffuse = false;
		int32 bNoise = false;
		int32 bMask = false;
		int32 bDistortion = false;

		int32 bDissolve = false;
		float fTrackPosition = 0;
		Vector2 vLifeTime;

		int32 bUVMoveLoop = false;
		Vector2 vUVAcc;
		int32 bSprite = 0;

		Vector2 vMinUV;
		Vector2 vMaxUV;

		int32 bGlow = false;
		float fGlowWeight = 0;
		int32 bBloom = false;
		float fBloomWeight = 0;

		int32 bAlphaAttenuation = false;
		float fAlphaAttenuationWeight = 0;
		float fDistortionWeight = 0;
		float fDissolveWeight = 0;

		Vector4 vDiffuseColor1;

		Vector4 vColor1;

		Vector4 vColor2;

		Vector4 vColor3;

		int32 bTrail = false;

		float fAlphaDiscardValue = 0.5f;
	};

	// PBR
	struct ENGINE_DLL FPBR final
	{
		Matrix matWorld;
		Matrix matView;
		Matrix matProj;
		FLightInfo tLightInfo;
	};

	// UI - �⺻
	struct ENGINE_DLL FUiInfo final
	{
		Matrix matWorld;

		Matrix matView;

		Matrix matProj;

		float fRatio = 0;
		int32 bAlpha = false;
		int32 bColor = false;
		int32 bMask = false;

		float fAlpha = 0;
		Vector3 vRectpad1;

		Vector4 vColor;
	};

	// UI - AnimationImage
	struct ENGINE_DLL FUiInfo_AnimationImage final
	{
		Vector2 vMinTexCoord = { 0.0f, 0.0f };
		Vector2 vMaxTexCoord = { 1.0f, 1.0f };
	};

	// UI - ProgressBar
	struct ENGINE_DLL FUiInfo_ProgressBar final
	{
		Vector4 vOtherColor = Vector4::One;

		float fOtherRatio = 0.0f;
		Vector3 vProgressBarPad1;
	};

	// Effect - Particle
	struct ENGINE_DLL FEffectInfo_Particle final
	{
		Matrix matWorld;

		int32 bDiffuse = false;
		int32 bMask = false;
		int32 bBillboard = false;
		int32 bDirectional = false;

		float fSizeRatio = 0;
		Vector2 vSize;
		float fParticlePad1 = 0;

		Vector4 vColor1;

		Vector4 vColor2;

		Vector4 vColor3;

		float fTrackPosition = 0.f;
	};

	// Effect - Trail
	struct ENGINE_DLL FEffectInfo_Trail final
	{
		Matrix matWorld;

		int32 bUseDiffuse = false;
		int32 bUseNoise = false;
		int32 bUseMask = false;
		int32 bUseDistortion = false;

		int32 bUseDissolve = false;
		Vector3 vTrailPad1;

		Vector4 vColor;
	};

	// Effect - Deffered
	struct ENGINE_DLL FEffectInfo_Deffered final
	{
		Matrix matWorld;

		Matrix matView;

		Matrix matProj;

		float fDistortionTimer = 0;
		Vector3 vEffectDefferedPad1;
	};
}