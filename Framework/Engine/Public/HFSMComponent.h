#pragma once
#include "Component.h"

namespace Engine
{
	class CHStateGroup;
	class CHState;

	/********************
		Description
	********************/
	struct FHFSMComponentDesc : public CComponent::FComponentDesc
	{

	};

	class ENGINE_DLL CHFSMComponent: public CComponent
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CHFSMComponent() = default;

		virtual ~CHFSMComponent() = default;
		
	/********************
		Events
	********************/
	public:
		static std::shared_ptr<CHFSMComponent> Create(const std::shared_ptr<FHFSMComponentDesc>& _spHFSMComponentDesc);

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
		HRESULT AddHGroup(const std::shared_ptr<CHStateGroup>& _spHStateGroup);

		HRESULT RemoveHGroup(int32 _iHStateGroupIndex);

	/********************
		Getter/Setter
	********************/
	public:
		void ReserveCommand(const std::vector<std::tuple<int32, int32>>& _vecCommand, EHFSMReserveCommandType _eReserveCommandType);

	/********************
		Data Members
	********************/
	private:
		// 명령 큐
		std::deque<std::vector<std::tuple<int32, int32>>> m_dqCommands;

		// 현재 실행 중인 명령 인덱스
		int32 m_iCurCommandIndex = 0;

		std::unordered_map<int32, std::shared_ptr<CHStateGroup>> m_umapHStateGroups;
	};
}
