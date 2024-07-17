#pragma once
#include "Component.h"

namespace Engine
{
	class CPointBuffer;
	class CEffectShader;

	class ENGINE_DLL CRectComponent : public CComponent
	{
		/********************
			Description
		********************/
	public:
		struct ENGINE_DLL FRectComponentDesc : public FComponentDesc
		{
			std::wstring m_wstrShaderFileName = L"";
			std::wstring m_wstrMaskTextureFileName = L"";
			EUIType m_eUIType = EUIType::ENUM_END;
			uint32 m_iPassIndex = 0;
			uint32 m_iDepth = 0;
			Vector4 m_vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			float m_fAlpha = 1.0f;
			float m_fRectAngle = 0.0f;
			bool m_bAlpha = false;
			bool m_bColor = false;
			bool m_bMask = false;

			bool m_bInteraction = false;
		};

		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CRectComponent() = default;

		virtual ~CRectComponent() = default;

		/********************
			Events
		********************/
	public:
		static std::shared_ptr<CRectComponent> Create(const std::shared_ptr<FRectComponentDesc>& _spRectComponentDesc);

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
		// ���콺�� Rect �ȿ� �ִ��� �Ǵ��մϴ�.
		bool IsInMouse();
		// ���콺�� �������� ������ �����ݴϴ�.
		void AddMouseMove();
		// ���콺�� ó�� Rect�ȿ� �ִ��� �Ǵ��մϴ�.
		void CheckClickInRect();

		void CheckMouseDown();

		virtual bool IsDragged() { return false; };

		Matrix ChangeRotationMatrix(float _fDegree);

		Vector2 CalculateViewportPosition(Vector3 _vPosition);

		void CenterToRelativeCenter();

		void WorldToRelativeCenter();

		void SortRelativeCenterToCenter();

		Vector3 SortCameraRelativeToCenter();

		void CalculateScreenRelativeCenterToCenter();

		void CalculateOrthgonalScreenRelativeCenter();

		/********************
			Getter/Setter
		********************/
	public:
		void SetShaderFileName(const std::wstring& _wstrShaderFileName) { m_wstrShaderFileName = _wstrShaderFileName; }

		std::wstring GetShaderFileName() const { return m_wstrShaderFileName; }

		void SetMaskTextureFileName(const std::wstring& _wstrMaskTextureName);

		std::wstring GetMaskTextureFileName() const { return m_wstrMaskTextureFileName; }

		uint32 GetPassIndex() const { return m_iPassIndex; }

		void SetCenterPosition(const Vector2& _vCenter) { m_vCenter = _vCenter; }

		void SetWorldtoCenterPosition(const Vector4& _vPosition);

		Vector3 GetRectWorldPosition(Vector3 _vCenter);

		Vector2 GetCenterPosition() const { return m_vCenter; }

		void SetRect(RECT _rcRect) { m_tRect = _rcRect; }

		RECT GetRect() const { return m_tRect; }

		void SetDepth(uint32 _iDepth) { m_iDepth = _iDepth; }

		uint32 GetDepth() { return m_iDepth; }

		void SetUIType(EUIType _eUIType) { m_eUIType = _eUIType; }

		EUIType GetUIType() const { return m_eUIType; }

		void SetInteraction(float _bInteraction) { m_bInteraction = _bInteraction; }

		bool GetInteraction() const { return m_bInteraction; }

		void SetFocusedControl(float _bFocusedControl) { m_bFocusedControl = _bFocusedControl; }

		bool GetFocusedControl() const { return m_bFocusedControl; }

		void SetRatio(float _fRatio) { m_fRatio = _fRatio; }

		float GetRatio() const { return m_fRatio; }

		void SetColor(const Vector4& _vColor) { m_vColor = _vColor; }

		Vector4 GetColor() const { return m_vColor; }

		void SetPreMouse(pair<int32, int32> _prMouse) { m_prPreMouse = _prMouse; }

		pair<int32, int32> GetPreMouse() const { return m_prPreMouse; }

		void SetAlpha(float _fAlpha) { m_fAlpha = _fAlpha; }

		float GetAlpha() const { return m_fAlpha; }

		void SetBoolAlpha(bool _bAlpha) { m_bAlpha = _bAlpha; }

		bool GetBoolAlpha() const { return m_bAlpha; }

		void SetBoolMask(bool _bMask) { m_bMask = _bMask; }

		bool GetBoolMask() const { return m_bMask; }

		void SetBoolColor(bool _bColor) { m_bColor = _bColor; }

		bool GetBoolColor() const { return m_bColor; }

		void SetRectAngle(float _fRectAngle) { m_fRectAngle = _fRectAngle; }

		float GetRectAngle() const { return m_fRectAngle; }

		void SetInterraction(bool _bInteraction) { m_bInteraction = _bInteraction; }

		bool IsInteraction() const { return m_bInteraction; }

		void SetMouseReset(bool _bMouseReset) { m_bMouseReset = _bMouseReset; }

		void SetWorldRelativeCenter(Vector3 _vWorldRelativeCenter) { m_vWorldRelativeCenter = _vWorldRelativeCenter; }

		const Vector3 GetWorldRelativeCenter() const { return m_vWorldRelativeCenter; }

		void SetScreenRelativeCenter(Vector2 _vScreenRelativeCenter) { m_vScreenRelativeCenter = _vScreenRelativeCenter; }

		const Vector2 GetScreenRelativeCenter() const { return m_vScreenRelativeCenter; }

		void SetWorldToScreenPosition(Vector2 _vWorldToScreenPosition) { m_vWorldToScreenPosition = _vWorldToScreenPosition; }

		const Vector2 GetWorldToScreenPosition() const { return m_vWorldToScreenPosition; }

		void IsTool(bool _bTool) { m_bTool = _bTool; }

		const bool IsTool() const { return m_bTool; }

		void IsRelativeCenter(bool _bRelativeCenter) { m_bRelativeCenter = _bRelativeCenter; }

		const bool IsRelativeCenter() const { return m_bRelativeCenter; }

	public:
		void SetRectScale(const Vector3& _vScale);

		void SetRectRotate(const Vector3& _vAxis, float _fDegree);

		void SetRectRotate(const Vector4& _vRotation);

		void SetRectRotate(const Matrix& _matRotation);

		void SetRectPosition(const Vector3& _vTranlate);

		void SetWorldMatrix(const Matrix& _WorldMatrix);

	public:
		Vector3 GetRectScale();

		Vector3 GetRectRotate();

		Vector3 GetRectPosition();

		Matrix GetWorldMatrix();

	public:
		void SetRectTransform(ETransformType _eTransformType, const Vector4& _vTransform);

		Vector4 GetRectTransform(ETransformType _eTransformType) const;

		/********************
			Data Members
		********************/
	protected:
		std::shared_ptr<CPointBuffer> m_spPointBuffer = nullptr;

		std::weak_ptr<CEffectShader> m_wpEffectShader;

		std::wstring m_wstrShaderFileName = L"";

		//Shader�� ������ ���� ����� �����մϴ�.
		Matrix m_matRectWorld = Matrix::Identity;

		// Shader�� ������ �� ����� �����մϴ�.
		Matrix m_matView = Matrix::Identity;

		// Shader�� ������ ���� ����� �����մϴ�.
		Matrix m_matProjection = Matrix::Identity;

		// ����Ʈ �����ӿ�ũ�� �н��� �����մϴ�.
		uint32 m_iPassIndex = 0;

		// ���� ������ �����մϴ�.
		uint32 m_iDepth = 0;

		// ������ �����մϴ�.
		Vector2 m_vCenter = Vector2::Zero;

		// ��� ��ǥ�� �����մϴ�.
		Vector2 m_vScreenRelativeCenter = Vector2::Zero;

		// ��� ���� ��ǥ�� �����մϴ�.
		Vector3 m_vWorldRelativeCenter = Vector3::Zero;

		// ������ ����� �������� RECT�� �����մϴ�.
		RECT m_tRect = {};

		// UIŸ���� �����մϴ�.
		EUIType m_eUIType = EUIType::ENUM_END;

		// ��ȣ�ۿ��ϴ� ������Ʈ���� üũ�մϴ�.
		bool m_bInteraction = false;

		// ó�� Ŭ�� ���θ� üũ�մϴ�. // �ٸ� Rect component�� ��ȣ�ۿ������� �Ǵ��մϴ�.
		bool m_bFocusedControl = false;

		// rect�� ������ �����մϴ�.
		float m_fRatio = 1.0f;

		// ������ �����մϴ�.
		Vector4 m_vColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		// ���콺�� ���� ������ ��ġ�� �����մϴ�.
		pair<int32, int32> m_prPreMouse;

		float m_fAlpha = 1.0f;
		// ������ �����մϴ�.
		float m_fRectAngle = 0.0f;

		// ���� ��ǥ�� ��ũ�� ��ǥ�� �ø� ���� �����մϴ�.
		Vector2 m_vWorldToScreenPosition = Vector2::Zero;

	protected:
		// Bind
		FUiInfo m_tUiInfo = {};

		// Bind Variables
		ComPtr<ID3DX11EffectVariable> m_cpVariable_UiInfo = nullptr;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_cpVariable_Textures = nullptr;

		//�ؽ��ĸ� �־��ݴϴ�.
		vector<ID3D11ShaderResourceView*> m_vecSRVs;

		std::wstring m_wstrMaskTextureFileName = L"";

		// ����ũ ��� ���θ� �����մϴ�.
		bool m_bMask = false;
		// ���İ� ��� ���θ� �����մϴ�.
		bool m_bAlpha = false;
		// �÷��� ��� ���θ� �����մϴ�.
		bool m_bColor = false;

		bool m_bMouseReset = false;
		// ������ �ƴ����� ���θ� �����մϴ�.
		bool m_bTool = false;
		// ��� ��ǥ�� ó���� ������ ������ǥ�� ó���� �������� ���θ� üũ�մϴ�.
		bool m_bRelativeCenter = false;

		float m_fRectDeltaSeconds = 0.0f;
	};
}


