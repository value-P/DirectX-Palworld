#pragma once
#include "UI.h"

namespace Engine
{
	class CRectComponent;

	class ENGINE_DLL CPanel : public CUI
	{
	/********************
		Description
	********************/
	public:
		struct FPanelDesc : public FUIDesc
		{
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPanel() = default;

		virtual ~CPanel() = default;

	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CPanel> Create(const std::shared_ptr<FPanelDesc>& _spPanelDesc);

		static std::shared_ptr<CPanel> CreateFromJson(const std::wstring& _wstrJsonFileName);

		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

	public:
		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

	/********************
		Methods
	********************/
	public:
		std::shared_ptr<CRectComponent> GetFindSelectRectComponent();

		const std::map<std::wstring, std::shared_ptr<CComponent>>& GetMapComponents() { return m_mapComponents; }

		std::shared_ptr<CGameObject> FindChildGameObject(const std::wstring& _wstrGameObjectName);

		void LoadJson(shared_ptr<CPanel> _spPanel, rapidjson::Document& _doc, int32& iPanelIndex);

	/********************
		Getter/Setter
	********************/
	public:
		const std::vector<std::shared_ptr<CGameObject>>& GetVecChildGameObjects() { return m_vecChildGameObjects; }

		void SetDepth(int32 _iDepth) { m_iDepth = _iDepth; }

		const int32 GetDepth()& { return m_iDepth; }

		void SetPivotComponent(shared_ptr<CRectComponent> _spPivotComponent) { m_spPivotComponent = _spPivotComponent; }

		const std::shared_ptr<CRectComponent> GetPivotComponent();

		void SetPivotWorldPoint(Vector3 _vPivotWorldPoint);

		const Vector3 GetPivotWorldPoint();

		void SetPivotCenterPoint(Vector2 _vPivotCenterPoint);

		const Vector2 GetPivotCenterPoint();

	/********************
		Data Members
	********************/
	private:
		int32 m_iDepth = 0;

		std::shared_ptr<CRectComponent> m_spPivotComponent = nullptr;
	};
}