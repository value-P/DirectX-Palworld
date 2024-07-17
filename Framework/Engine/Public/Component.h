#pragma once
namespace Engine
{
	class CGameObject;
	class CWorld;

	class ENGINE_DLL CComponent : public enable_shared_from_this<CComponent>
	{
		/********************
			Description
		********************/
	public:
		struct ENGINE_DLL FComponentDesc : public enable_shared_from_this<FComponentDesc>
		{
			// �� ������Ʈ�� ���� �ִ� ���� ������Ʈ
			std::weak_ptr<CGameObject> m_wpOwnerGameObject;

			// �� ������Ʈ�� Ÿ��
			EComponentType m_eComponentType = EComponentType::ENUM_END;

			// ������Ʈ �̸�
			wstring m_wstrComponentName = L"";

			// ������Ʈ ���� Ÿ��
			ERenderType m_eRenderType = ERenderType::ENUM_END;
		};

		/********************
			Ctor/Dtor
		********************/
	public:
		explicit CComponent() = default;

		virtual ~CComponent() = default;

		/********************
			Events
		********************/
	public:
		virtual HRESULT InitializeComponent(const std::shared_ptr<FComponentDesc>& _spComponentDesc);

		virtual HRESULT BeginPlay() = 0;

		virtual int32 PreTickComponent(float _fDeltaSeconds) = 0;

		virtual int32 TickComponent(float _fDeltaSeconds) = 0;

		virtual int32 PostTickComponent(float _fDeltaSeconds) = 0;

		virtual HRESULT EndPlay() = 0;

		virtual void Render(int _iShaderPath = 0) = 0;

		virtual void Release() = 0;

		virtual HRESULT BindVariable() = 0;

		/********************
			Getter/Setter
		********************/
	public:
		void SetOwnerGameObject(std::shared_ptr<CGameObject> _spOwnerGameObject) { m_wpOwnerGameObject = _spOwnerGameObject; }

		const std::shared_ptr<CGameObject> GetOwnerGameObject() const;

		void SetComponentName(const std::wstring& _wstrComponentName) { m_wstrComponentName = _wstrComponentName; }

		const std::wstring& GetComponentName() const { return m_wstrComponentName; }

		void SetComponentType(EComponentType _eComponentType) { m_eComponentType = _eComponentType; }

		EComponentType GetComponentType() const { return m_eComponentType; }

		virtual void SetCulled(bool _bCulled) { m_bCulled.store(_bCulled); }

		bool IsCulled() const { return m_bCulled.load(); }

		virtual void SetActive(bool _bActive) { m_bActive.store(_bActive); }

		bool IsActive() const { return m_bActive.load(); }

		ERenderType GetRenderType() const { return m_eRenderType; }

		/********************
			Data Members
		********************/
	protected:
		// �� ������Ʈ�� ���� �ִ� ���� ������Ʈ
		std::weak_ptr<CGameObject> m_wpOwnerGameObject;

		// �� ������Ʈ�� Ÿ��
		EComponentType m_eComponentType = EComponentType::ENUM_END;

		// ������Ʈ �̸�
		std::wstring m_wstrComponentName = L"";

		// ������Ʈ ���� Ÿ��
		ERenderType m_eRenderType = ERenderType::ENUM_END;

		// ������Ʈ ���� ����
		std::atomic<bool> m_bCulled = false;

		// ������Ʈ Ȱ��ȭ(ƽ, ����) ����
		std::atomic<bool> m_bActive = true;
	};
}
