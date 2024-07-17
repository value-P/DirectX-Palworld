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
			// 이 컴포넌트를 갖고 있는 게임 오브젝트
			std::weak_ptr<CGameObject> m_wpOwnerGameObject;

			// 이 컴포넌트의 타입
			EComponentType m_eComponentType = EComponentType::ENUM_END;

			// 컴포넌트 이름
			wstring m_wstrComponentName = L"";

			// 컴포넌트 렌더 타입
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
		// 이 컴포넌트를 갖고 있는 게임 오브젝트
		std::weak_ptr<CGameObject> m_wpOwnerGameObject;

		// 이 컴포넌트의 타입
		EComponentType m_eComponentType = EComponentType::ENUM_END;

		// 컴포넌트 이름
		std::wstring m_wstrComponentName = L"";

		// 컴포넌트 렌더 타입
		ERenderType m_eRenderType = ERenderType::ENUM_END;

		// 컴포넌트 렌더 여부
		std::atomic<bool> m_bCulled = false;

		// 컴포넌트 활성화(틱, 렌더) 여부
		std::atomic<bool> m_bActive = true;
	};
}
