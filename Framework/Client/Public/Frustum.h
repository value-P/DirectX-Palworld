#pragma once

namespace Engine
{
	class CCameraManager;
}

namespace Client
{
	class CFrustum final
	{
	public:
		explicit CFrustum() = default;

		virtual ~CFrustum() = default;

	public:
		static std::shared_ptr<CFrustum> Create();

	public:
		HRESULT Initialize();

		int32 Tick();

	private:
		std::array<Vector3, 8> m_arrLocalPoints{ Vector3(0,0,0) };

		std::array<Vector3, 8> m_arrWorldPoints{ Vector3(0,0,0) };

		std::array<Vector4, 6> m_arrWorldPlanes{ Vector4(0,0,0,0) };

		std::weak_ptr<CCameraManager> m_wpCameraManager;
	};
}
