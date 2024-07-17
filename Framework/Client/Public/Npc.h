#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMeshComponent;
	class CBoxComponent;
	class CPanel;
	class CTextComponent;
	class CSoundManager;
}

namespace Client
{
	class CQuestSystem;
	class CInventorySystem;
	class CUIManager;
	class CPlayer;

	struct FNpcTextStruct
	{
		wstring m_wstrText = L"";		// �ؽ�Ʈ ����
		function<void()> m_funcEvent;	// �ش� �ؽ�Ʈ���� �����ϰ��� �ϴ� �Լ�(���鰡��)
		int32 m_iAnimID = -1;			// �ش� �ؽ�Ʈ���� ����ϰ��� �ϴ� �ִϸ��̼� ID(���鰡��, �ѹ��� ����ǰ� Idle�� ���ư�)
	};

	struct FNpcCycle
	{
		bool Next()
		{
			if (m_iCurrentTextIndex >= m_vecNpcCycle.size() - 1) 
			{
				m_bIsLast = true;
				return false; 
			}

			m_iCurrentTextIndex++;
			return true;
		}
		void Reset() { m_iCurrentTextIndex = 0; }
		std::optional<FNpcTextStruct> Current()
		{
			if (m_vecNpcCycle.empty()) { return nullopt; }

			return m_vecNpcCycle[m_iCurrentTextIndex];
		}
		bool IsLast() { return m_bIsLast; }

		vector<FNpcTextStruct> m_vecNpcCycle;
		int32 m_iCurrentTextIndex = 0;
		bool m_bIsLast = false;
	};

	struct FNpcDesc : CGameObject::FGameObjectDesc
	{
		wstring m_wstrNpcName = L"";
		Vector3 m_vInitialLook = Vector3(0.f, 0.f, 1.f);
		ENpcModelType m_eModelType = ENpcModelType::ENUM_END;
	};

    class CNpc abstract : public CGameObject
    {
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CNpc() = default;

		virtual ~CNpc() = default;

	/********************
		Events
	********************/
	public:
		virtual HRESULT Initialize(const std::shared_ptr<FGameObjectDesc>& _spGameObjectDesc) override;

		virtual HRESULT BeginPlay() override;

		virtual int32 PreTick(float _fDeltaSeconds) override;

		virtual int32 Tick(float _fDeltaSeconds) override;

		virtual int32 PostTick(float _fDeltaSeconds) override;

		virtual HRESULT EndPlay() override;

		virtual void Release() override;

		virtual void OnTriggerEnter(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;
		virtual void OnTriggerExit(CComponent* _pSrcComponent, CComponent* _pDstComponent) override;

	protected:
		virtual void InitCycle() = 0;

	private:
		void InitMeshComponent(const std::shared_ptr<FNpcDesc>& _spGameObjectDesc);
		void InitColliderBox();

	/********************
		Getter/Setter
	********************/
	public:
		bool IsDialogActivate() { return m_bDialogActivate; }

	/********************
		Methods
	********************/
	public:
		virtual void Interact();
		// �ܺο����� �����ؾ��ؼ� ������ �����ϴ�. //������
		void ChangeCycle(wstring _wstrCycleName);

	protected:
		void PlayText(); // ������ �ؽ�Ʈ�� ���� true ��ȯ
		void PlayDialog();
		void EndDialog();

		void CoumputeLook();
		void ComputeAnimation();

		void ChangeLookToInitial();
		void ChangeDefaultAnimation(ENPCANIM _eNpcAnim);

		void AddExp();
		void NpcPlaySound(const string& _strSoundID);

	/********************
		Data Members
	********************/
	protected:
		weak_ptr<CQuestSystem> m_wpQuestSystem;
		weak_ptr<CUIManager> m_wpUIManager;
		weak_ptr<CInventorySystem> m_wpInventorySystem;
		weak_ptr<CSoundManager> m_wpSoundManager;

		weak_ptr<CPanel> m_wpDialogPanel;
			weak_ptr<CTextComponent> m_wpNameTextComponent;
			weak_ptr<CTextComponent> m_wpDialogTextComponent;

		std::weak_ptr<CPanel> m_wpRidingUI;

		weak_ptr<CGameObject> m_wpInteractObject;
		weak_ptr<CPlayer> m_wpPlayer;

		shared_ptr<CMeshComponent> m_spMeshComponent = nullptr;
		shared_ptr<CBoxComponent> m_spBoxCollider = nullptr;

		Vector3 m_vInitialLook = Vector3(0.f, 0.f, 0.f);
		Vector3 m_vDstLook = Vector3(0.f,0.f,0.f);

		ENPCANIM m_eDefaultAnim = ENPCANIM::AS_Player_Female_Idle_None;

		bool m_bDialogActivate = false;
		bool m_bInteracted = false;
		bool m_bCycleChanged = false;

		wstring m_wstrNpcName = L"";
		wstring m_wstrCurrentCycle = L"";
		unordered_map<wstring, FNpcCycle> m_umapCycle;

		EChannelType m_eCurrentChannel = EChannelType::ENUM_END;
    };
}