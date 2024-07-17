
namespace Engine
{
	struct ENGINE_DLL FImageAnimationFrameDesc
	{
		Vector4 vColor;

		Vector3 vPosition;

		Vector3 vRelativeCenter;

		Vector3 vScale;
		
		float fAlpha = 1.0f;

		float fSpeed = 0.0f;

		float fAngle = 0.0f;

		float fTime = 0.0f;
	};

	struct ENGINE_DLL FFontDesc
	{
		std::wstring m_wstrText = L"";
		
		Vector2 m_vScreenPosition = Vector2::Zero;

		Vector4 m_vColor = Vector4::One;

		float m_fRadian = 0.0f;

		Vector2 m_vOrigin = Vector2::Zero;

		float m_fScale = 1.0f;

		int32 m_iOption = 0x0;
	};
}
