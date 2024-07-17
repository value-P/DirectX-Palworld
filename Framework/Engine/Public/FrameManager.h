#pragma once
#include "Manager.h"

namespace Engine
{
	class CFrame;

	class ENGINE_DLL CFrameManager final : public CManager
	{
	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		float Tick(EFrameLoopType _eFrameLoopType);

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	private:
		std::shared_ptr<CFrame> CreateFrame(EFrameLoopType _eFrameLoopType);

		HRESULT AddFrame(EFrameLoopType _eFrameLoopType, const std::shared_ptr<CFrame>& _spFrame);

		HRESULT RemoveFrame(EFrameLoopType _eFrameLoopType);

	public:
		std::shared_ptr<CFrame> FindFrame(EFrameLoopType _eFrameLoopType);

	/********************
		Getter/Setter
	********************/
	public:
		std::optional<float> GetDeltaSeconds(EFrameLoopType _eFrameLoopType);

	/********************
		Data Members
	********************/
	private:
		std::unordered_map<EFrameLoopType, std::shared_ptr<CFrame>> m_umapFrames;
	};
}
