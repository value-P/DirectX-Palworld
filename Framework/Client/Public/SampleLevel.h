#pragma once
#include "LightManager.h"
#include "Level.h"

namespace Engine
{
	class CCamera;
	// �̹��� ȣ�� �׽�Ʈ�� ���� �ӽ� �ڵ��Դϴ�.
	class CPanel;
}

namespace Client
{
	class CPlayer;
	class CNpc;
	class CLandscape;
	class CEnvMesh;
	class CSheepBall;
	class CCustomizeObject;
	class CPinkCat;

	/*24-04-16 ���� �Ĺ� ������ ���� �׽�Ʈ�� �ڵ��Դϴ�. */ 
	class CTree;
	class CInstanceTree;
	class CRock;

	/* 24-04-22 �κ��丮 UI �׽�Ʈ�� Ŭ���� �Դϴ�. �̵��� �߰� */
	class CInventorySystem;

	class CSampleLevel final : public CLevel
	{
	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize() override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	public:
		void AddSheepBall();

		void RemovESHEEPBALL();

	/********************
		Data Members
	********************/
	private:
		std::shared_ptr<CPlayer> m_spSamplePlayer = nullptr;

		std::shared_ptr<CNpc> m_spSampleNpc = nullptr;

		std::shared_ptr<CCustomizeObject> m_spCustomizeObject = nullptr;

		std::shared_ptr<CLandscape> m_spSampleLandscape = nullptr;

		std::shared_ptr<CSheepBall> m_spSheep = nullptr;

		std::shared_ptr<CPinkCat> m_spPinkCat = nullptr;

		std::shared_ptr<CCamera> m_spCamera = nullptr;

		std::vector<std::shared_ptr<CEnvMesh>> m_vecEnvMeshes;

	private:
		/*24-04-16 ���� �Ĺ� ������ ���� �׽�Ʈ�� �ڵ��Դϴ�. */
		std::shared_ptr<CInstanceTree> m_spInstanceTree = nullptr;
		std::shared_ptr<CTree> m_spTree = nullptr;

		std::shared_ptr<CRock> m_spRock = nullptr;

		std::vector<std::shared_ptr<CSheepBall>> m_vecSheepBalls;
		std::shared_ptr<CInventorySystem> m_spInventorySystem = nullptr;

		// �̹��� ȣ�� �׽�Ʈ�� ���� �ӽ� �ڵ��Դϴ�.
		std::shared_ptr<CPanel> m_spTestPanel = nullptr;
	};
};