#pragma once
#include "Component.h"

namespace Engine
{
	class ENGINE_DLL CTransformComponent : public CComponent
	{
		enum
		{
			RIGHT,
			UP,
			FORWARD,
			POSITION,
			ENUM_END
		};

	/********************
		Description
	********************/
	public:
		struct ENGINE_DLL FTransformComponentDesc : public FComponentDesc
		{
			Vector3 m_vLocalScale = Vector3::One;
			Vector3 m_vLocalRotate = Vector3::Zero;
			Vector3 m_vLocalTranslate = Vector3::Zero;
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CTransformComponent() = default;

		virtual ~CTransformComponent() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CTransformComponent> Create(const std::shared_ptr<FTransformComponentDesc>& _spTransformComponentDesc);

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
		void LookAt(const Vector3& _vPosition);

		void Turn(const Vector3& _vAxis, float _fAngularSpeed);

		void MoveForward(float _fSpeed);

		void MoveBackward(float _fSpeed);

		void MoveRight(float _fSpeed);

		void MoveLeft(float _fSpeed);

		void MoveUp(float _fSpeed);

		void MoveDown(float _fSpeed);

	/********************
		Getter/Setter
	********************/
	public:
		void SetScale(const Vector3& _vScale);

		void SetRotate(const Vector3& _vAxis, float _fDegree);

		void SetRotate(const Vector3& _vRotate);

		void SetRotateAccumulated(const Vector3& _vAxis, float _fDegree);

		void SetPosition(const Vector3& _vTranlate);

		void SetTransformMatrix(const Matrix& _matTransform);

	public:
		Vector3 GetScale();

		Vector3 GetRotate();

		Quaternion GetQuaternion();

		Vector3 GetPosition();

		const Matrix& GetTransformMatrix() const { return m_matTransform; }

	public:
		void SetTransform(ETransformType _eTransformType, const Vector4& _vTransform);

		Vector4 GetTransform(ETransformType _eTransformType) const;

	public:
		const Vector3 GetRightVector() const { return m_matTransform.Right(); }

		const Vector3 GetUpVector() const { return m_matTransform.Up(); }

		const Vector3 GetForwardVector() const { return -m_matTransform.Forward(); };

	/********************
		Data Members
	********************/
	protected:
		// 트랜스폼 행렬
		Matrix m_matTransform = Matrix::Identity;

		// 델타 세컨드
		float m_fDeltaSeconds = 0.0f;
	};
}
