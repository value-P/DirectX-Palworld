#pragma once
#include "Manager.h"

namespace Engine
{
	class CColliderComponent;
	class CSphereComponent;
	class CAABBComponent;
	class COBBComponent;

	class CSimulationEventCallback : public PxSimulationEventCallback
	{
	/********************
		Callback
	********************/
	public:
		void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;

		void onWake(PxActor** actors, PxU32 count) override;

		void onSleep(PxActor** actors, PxU32 count) override;

		void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;

		void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

		void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;
	
	public:
		static PxFilterFlags PxTriggerSimulationFilterShader
		(
			PxFilterObjectAttributes attributes0,
			PxFilterData filterData0,
			PxFilterObjectAttributes attributes1,
			PxFilterData filterData1,
			PxPairFlags& pairFlags,
			const void* constantBlock,
			PxU32 constantBlockSize
		);

		static bool IsFlagOverlapped(const PxFilterData& data0, const PxFilterData& data1);
	};

	class ENGINE_DLL CPhysXManager : public CManager
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CPhysXManager() = default;

		virtual ~CPhysXManager() = default;

	/********************
		Events
	********************/
	public:
		HRESULT Initialize();

		int32 Tick(float _fDeltaSeconds);

		virtual void Release() override;
		
	/********************
		Methods
	********************/
	private:
		HRESULT InitializePhysics();

		HRESULT InitializeScene();

	public:
		/*
		* @param
		* const Vector3&	_vPosition		: [in] 광선의 위치
		* const Vector3&	_vDirection		: [in] 광선의 방향
		* float				_fMaxDistance	: [in] 광선의 최대 거리
		* ECollisionLayer	_eCollisionLayer: [in] 광선이 감지할 충돌 레이어
		* PxRaycastHit*		_tHit			: [out] 광선과 충돌한 첫 번째 오브젝트
		* 
		* @return
		* 광선이 오브젝트와 충돌했는지 여부
		*/
		bool SingleRayCast
		(
			const Vector3& _vPosition,
			const Vector3& _vDirection,
			float _fMaxDistance,
			ECollisionLayer _eCollisionLayer,
			PxRaycastHit* _pRaycastHit
		);

		/*
		* @param
		* const Vector3&	_vPosition		: [in] 광선의 위치
		* const Vector3&	_vDirection		: [in] 광선의 방향
		* float				_fMaxDistance	: [in] 광선의 최대 거리
		* int32				_iCollisionFlag : [in] 광선이 감지할 충돌 레이어 집합
		* PxRaycastHit*		_tHit			: [out] 광선과 충돌한 첫 번째 오브젝트
		* 
		* @return
		* 광선이 오브젝트와 충돌했는지 여부
		*/
		bool SingleRayCast
		(
			const Vector3& _vPosition,
			const Vector3& _vDirection,
			float _fMaxDistance,
			int32 _iCollisionFlag,
			PxRaycastHit* _pRaycastHit
		);

		/*
		* @param
		* const Vector3&	_vPosition		: [in] 광선의 위치
		* const Vector3&	_vDirection		: [in] 광선의 방향
		* float				_fMaxDistance	: [in] 광선의 최대 거리
		* ECollisionLayer	_eCollisionLayer: [in] 광선이 감지할 충돌 레이어
		* PxRaycastHit*		_tHit			: [out] 광선과 충돌한 오브젝트들
		*
		* @return
		* 광선과 충돌한 오브젝트의 수
		* 
		* @detail
		* _tHit는 충돌할 오브젝트 수만큼 미리 resize 후에 매개 변수로 전달해야 합니다.
		*/
		int32 MultiRayCast
		(
			const Vector3& _vPosition,			
			const Vector3& _vDirection,
			float _fMaxDistance,
			ECollisionLayer _eCollisionLayer,
			std::vector<PxRaycastHit>& _vecRaycastHits
		);

		int32 MultiRayCast
		(
			const Vector3& _vPosition,
			const Vector3& _vDirection,
			float _fMaxDistance,
			int32 _iCollisionFlag,
			std::vector<PxRaycastHit>& _vecRaycastHits
		);


		// 테스트 미진행
		int32 BoxOverlap
		(
			const Vector3& _vExtents,
			const Vector3& _vPosition,
			const Vector3 _vRotate,
			ECollisionLayer _eCollisionLayer,
			std::vector<PxOverlapHit>& _vecOverlapHits
		);

		// 테스트 미진행
		int32 SphereOverlap
		(
			float _fRadius,
			const Vector3& _vPosition,
			const Vector3 _vRotate,
			ECollisionLayer _eCollisionLayer,
			std::vector<PxOverlapHit>& _vecOverlapHits
		);

	/********************
		Helper
	********************/
	public:
		static PxTransform GetPxTransformFromMatrix(Matrix& _matTransform);

		static Matrix GetMatrixFromPxTransform(PxTransform& _pxTransform);

		static Vector3 GetVector3FromPxVec3(PxVec3& _pxVec3);

		static PxVec3 GetPxVec3FromVector3(Vector3& _vVector3);

		static PxQuat GetPxQuatFromQuaternion(Quaternion& _qtQuaternion);

	/********************
		Getter/Setter
	********************/
	public:
		PxPhysics* GetPhysics() const { return m_pPhysics; }
		
		PxScene* GetScene() const { return m_pScene; }

		PxControllerManager* GetControllerManager() const { return m_pControllerManager; }

	/********************
		Data Members
	********************/
	private:
		PxDefaultAllocator m_oAllocator;

		PxDefaultErrorCallback m_oErrorCallback;

		PxFoundation* m_pFoundation = nullptr;

		PxDefaultCpuDispatcher* m_pDispatcher = nullptr;

		PxPvd* m_pPvd = nullptr;

		PxCudaContextManager* m_pCudaContextManager = nullptr;

	private:
		PxControllerManager* m_pControllerManager = nullptr;

		PxPhysics* m_pPhysics = nullptr;

		PxScene* m_pScene = nullptr;

	private:
		CSimulationEventCallback m_oSimulationEventCallBack;
	};
}
