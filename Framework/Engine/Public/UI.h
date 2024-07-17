#pragma once
#include "GameObject.h"

namespace Engine
{
	class ENGINE_DLL CUI abstract : public CGameObject
	{
	public:
		struct ENGINE_DLL FUIDesc : public FGameObjectDesc
		{

		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CUI() = default;

		virtual ~CUI() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Getter/Setter
	********************/
	public:
		void SetActive(bool _bActive) { m_bActive = _bActive; }

		bool GetActive() const { return m_bActive; }

		void SetDepth(uint32 _iDepth) { m_iDepth = _iDepth; }

		uint32 GetDepth() const { return m_iDepth; }

		void SetPivot(Vector2 _vPivot) { m_vPivot = _vPivot; }

		Vector2 GetPivot() const { return m_vPivot; }

		void SetWidth(float _fWidth) { m_fWidth = _fWidth; }

		void SetHeight(float _fHeight) { m_fHeight = _fHeight; }

		float GetWidth() const { return m_fWidth; }

		float GetHeight() const { return m_fHeight; }

		Matrix GetViewMatrix() const { return m_matView; }

		Matrix GetProjectionMatrix() const { return m_matProjection; }

		void SetUIProjectionType(EProjectionType _eUIProjectionType) { m_eProjectionType = _eUIProjectionType; }

		EProjectionType GetUIProjectionType() const { return m_eProjectionType; }

		void SetUIType(EUIType _eUIType) { m_eUIType = _eUIType; }

		EUIType GetUIType() const { return m_eUIType; }

	public:
		void SetShaderName(const std::wstring& _wstrShaderName) { m_wstrShaderName = _wstrShaderName; }

	/********************
		Data Members
	********************/
	protected:
		// ���� ������ �����մϴ�.
		uint32 m_iDepth = 0;

		// ������ �����մϴ�.
		Vector2 m_vPivot = Vector2::Zero;

		// ���� ����� �����մϴ�.
		float m_fWidth = 1.0f;

		// ���� ����� �����մϴ�.
		float m_fHeight = 1.0f;
 		//���� ������Ʈ�� �ҼӵǾ��ִ� ������Ʈ���� ���� ViewMatrix�Դϴ�.
		Matrix m_matView = Matrix::Identity;

		//���� ������Ʈ�� �ҼӵǾ��ִ� ������Ʈ���� ���� ViewMatrix�Դϴ�.
		Matrix m_matProjection = Matrix::Identity;

		// ���ٰ� ������ �����մϴ�.
		EProjectionType m_eProjectionType = EProjectionType::ENUM_END;

		// UI Ÿ���� �����մϴ�.
		EUIType m_eUIType = EUIType::ENUM_END;

	protected:
		std::wstring m_wstrShaderName = L"";
	};
}
