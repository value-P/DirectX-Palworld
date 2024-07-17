#pragma once

namespace Engine
{
	class CMeshComponent;
	class CFSMComponent;
	class CModel;

	class ENGINE_DLL CFSMNode abstract
	{
	/********************
		Struct
	********************/
	protected:
		struct FAnimData
		{
			bool operator== (const FAnimData& _tOther)
			{
				bool result = true;

				result = m_iAnimID == _tOther.m_iAnimID &&
				m_bUseReversePlay == _tOther.m_bUseReversePlay &&
				m_fLoopRatio == _tOther.m_fLoopRatio &&
				m_fLerpTime == _tOther.m_fLerpTime;

				return result;
			}

			bool operator!= (const FAnimData& _tOther)
			{
				return !(*this == _tOther);
			}

			int32 m_iAnimID = -1;

			bool m_bUseReversePlay = false;

			float m_fLoopRatio = 0.f;

			float m_fLerpTime = 0.2f;
		};

	/********************
		Ctor/Dtor
	********************/
	public:
		CFSMNode() = default;

		virtual ~CFSMNode() = default;

	/********************
		Getter/Setter
	********************/
	public:
		bool GetUseReversePlay() { return m_bUseReversePlay; }

		void SetOwnerFSMComponent(const std::shared_ptr<CFSMComponent>& _spOwnerComponent);

	/********************
		Events
	********************/
	public:
		void Initialize(const std::shared_ptr<CMeshComponent>& _spMeshComponent);

	/********************
		Methods
	********************/
	public:
		virtual void OnEnterState() = 0;

		virtual void OnState(float _fDeltaSeconds) = 0;

		virtual void OnExitState() = 0;

		virtual void KeyInput() = 0;

	protected:
		virtual void ExpectNextAnim(FAnimData& _tDownAnimData, FAnimData& _tUpperAnimData) = 0;

	/********************
		Data Members
	********************/
	protected:
		std::weak_ptr<CFSMComponent> m_wpOwnerFSMComponent;

		std::weak_ptr<CMeshComponent> m_wpCombinedMeshComponent;

		std::weak_ptr<CModel> m_wpModel;
				
		bool m_bUseReversePlay = false;

		FAnimData m_tDownAnimData;

		FAnimData m_tUpperAnimData;
	};
}
