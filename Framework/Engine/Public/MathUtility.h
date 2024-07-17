#pragma once

namespace Engine
{
	class CMathUtility final
	{
	public:
		/*
		* @detail:	최소 실수와 최대 실수를 전달받아 그 사이의 무작위 실수를 반환합니다.
		*/
		static float GetRandomFloat(float _fMin, float _fMax)
		{
			if (_fMax < _fMin) { return 0.0f; }

			static std::random_device rd;
			static std::mt19937_64 generator(rd());
			std::uniform_real_distribution<float> fDistribution(_fMin, _fMax);
			return fDistribution(generator);
		}

		/*
		* @detail:	최소 정수와 최대 정수를 전달받아 그 사이의 무작위 정수를 반환합니다.
		*/
		static int32 GetRandomInt(int32 _iMin, int32 _iMax)
		{
			if (_iMax < _iMin) { return 0; }

			static std::random_device rd;
			static std::mt19937_64 generator(rd());
			uniform_int_distribution<int32> unDistribution(_iMin, _iMax);
			return unDistribution(generator);
		}

		static bool GetRandomBool(float _fProbability)
		{
			if (_fProbability <= 0.0f) { _fProbability = 0.0f; }
			if (1.0f <= _fProbability) { _fProbability = 1.0f; }

			static std::random_device rd;
			static std::mt19937_64 generator(rd());
			std::bernoulli_distribution bnDistribution(_fProbability);
			return bnDistribution(generator);
		}

	public:
		/* 스케일을 무시한 행렬을 반환합니다*/
		static Matrix GetScaledKilledMatrix(const Matrix& _matSrc)
		{
			Vector3 vRight = _matSrc.Right();
			Vector3 vUp = _matSrc.Up();
			Vector3 vLook = _matSrc.Backward();

			vRight.Normalize();
			vUp.Normalize();
			vLook.Normalize();

			Matrix matDst;
			matDst.Right(vRight);
			matDst.Up(vUp);
			matDst.Backward(vLook);
			matDst.Translation(_matSrc.Translation());

			return matDst;
		}

		/*
		* @detail
		* Vector3 타입 위치 벡터를 Vector4 타입으로 변경합니다.
		* 가령, Vector3(0.0f, 0.0f, 0.0f)를 Vector4(0.0f, 0.0f, 0.0f, 1.0f)로 변경합니다.
		*/
		static Vector4 PosVec3ToVec4(const Vector3& _vPosition)
		{
			return Vector4(_vPosition.x, _vPosition.y, _vPosition.z, 1.0f);
		}

		/*
		* @detail
		* Vector3 타입 방향 벡터를 전달받아 Vector4 타입으로 변경합니다.
		* 가령, Vector3(0.0f, 0.0f, 0.0f)를 Vector4(0.0f, 0.0f, 0.0f, 0.0f)로 변경합니다.
		*/
		static Vector4 DirVec3ToVec4(const Vector3& _vDirection)
		{
			return Vector4(_vDirection.x, _vDirection.y, _vDirection.z, 0.0f);
		}

	public:
		/*
		* 03.31 다양한 보간을 위해 추가했습니다. -동연-
		*/

		////////////////
		/* Slerp */
		////////////////
		static Vector3 Slerp(const Vector3& _vStart, const Vector3& _vEnd, float _fRatio)
		{
			
		}

		////////////////
		/* EaseInSine */
		////////////////
		static float EaseInSine(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			// _fRatio = sinf((_fRatio - 1.f) * XM_2PI) + 1.f;
			_fRatio = sinf(_fRatio * XM_2PI);

			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * _fRatio;

			return fInterpolatedValue;
		}
		static Vector3 EaseInSine(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			_fRatio = sinf((_fRatio - 1.f) * XM_2PI) + 1.f;

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * _fRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * _fRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * _fRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseInSine(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			_fRatio = sinf((_fRatio - 1.f) * XM_2PI) + 1.f;

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * _fRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * _fRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * _fRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * _fRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		/////////////////
		/* EaseOutSine */
		/////////////////
		static float EaseOutSine(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			_fRatio = sinf(_fRatio * XM_2PI);

			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * _fRatio;

			return fInterpolatedValue;
		}
		static Vector3 EaseOutSine(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			_fRatio = sinf(_fRatio * XM_2PI);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * _fRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * _fRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * _fRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseOutSine(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			_fRatio = sinf(_fRatio * XM_2PI);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * _fRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * _fRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * _fRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * _fRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		///////////////////
		/* EaseInOutSine */
		///////////////////
		static float EaseInOutSine(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * (-0.5f * cosf(_fRatio * XM_PI) + 0.5f);

			return fInterpolatedValue;
		}
		static Vector3 EaseInOutSine(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * (-0.5f * cosf(_fRatio * XM_PI) + 0.5f);
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * (-0.5f * cosf(_fRatio * XM_PI) + 0.5f);
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * (-0.5f * cosf(_fRatio * XM_PI) + 0.5f);

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseInOutSine(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * (-0.5f * cosf(_fRatio * XM_PI) + 0.5f);
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * (-0.5f * cosf(_fRatio * XM_PI) + 0.5f);
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * (-0.5f * cosf(_fRatio * XM_PI) + 0.5f);
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * (-0.5f * cosf(_fRatio * XM_PI) + 0.5f);

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		//////////////////
		/* EaseInSquare */// -> 매개변수에 제곱 계수가 추가됩니다.
		//////////////////
		static float EaseInSquare(float _fStart, float _fEnd, float _fRatio, float _fPower)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			
			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * powf(_fRatio, _fPower);

			return fInterpolatedValue;
		}
		static Vector3 EaseInSquare(Vector3 _vStart, Vector3 _vEnd, float _fRatio, float _fPower)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * powf(_fRatio, _fPower);
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * powf(_fRatio, _fPower);
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * powf(_fRatio, _fPower);

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseInSquare(Vector4 _vStart, Vector4 _vEnd, float _fRatio, float _fPower)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * powf(_fRatio, _fPower);
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * powf(_fRatio, _fPower);
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * powf(_fRatio, _fPower);
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * powf(_fRatio, _fPower);

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		///////////////////
		/* EaseOutSquare */
		///////////////////
		static float EaseOutSquare(float _fStart, float _fEnd, float _fRatio, float _fPower)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			float fEasedRatio = _fRatio - 1.f;

			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * (powf(fEasedRatio, _fPower) + 1.f);

			return fInterpolatedValue;
		}
		static Vector3 EaseOutSquare(Vector3 _vStart, Vector3 _vEnd, float _fRatio, float _fPower)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			float fEasedRatio = _fRatio - 1.f;

			float fInterpolatedValueX = _vStart.x + (_vEnd.x - _vStart.x) * (powf(fEasedRatio, _fPower) + 1.f);
			float fInterpolatedValueY = _vStart.y + (_vEnd.y - _vStart.y) * (powf(fEasedRatio, _fPower) + 1.f);
			float fInterpolatedValueZ = _vStart.z + (_vEnd.z - _vStart.z) * (powf(fEasedRatio, _fPower) + 1.f);

			return Vector3(fInterpolatedValueX, fInterpolatedValueY, fInterpolatedValueZ);
		}
		static Vector4 EaseOutSquare(Vector4 _vStart, Vector4 _vEnd, float _fRatio, float _fPower)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			float fEasedRatio = _fRatio - 1.f;

			float fInterpolatedValueX = _vStart.x + (_vEnd.x - _vStart.x) * (powf(fEasedRatio, _fPower) + 1.f);
			float fInterpolatedValueY = _vStart.y + (_vEnd.y - _vStart.y) * (powf(fEasedRatio, _fPower) + 1.f);
			float fInterpolatedValueZ = _vStart.z + (_vEnd.z - _vStart.z) * (powf(fEasedRatio, _fPower) + 1.f);
			float fInterpolatedValueW = _vStart.w + (_vEnd.w - _vStart.w) * (powf(fEasedRatio, _fPower) + 1.f);

			return Vector4(fInterpolatedValueX, fInterpolatedValueY, fInterpolatedValueZ, fInterpolatedValueW);
		}

		/////////////////////
		/* EaseInOutSquare */
		/////////////////////
		static float EaseInOutSquare(float _fRatio, float _fPower)
		{
			if (_fRatio < 0.5f)
				return 0.5f * powf(2.f * _fRatio, _fPower);
			else
				return 1.f - 0.5f * powf(2.f * (1.f - _fRatio), _fPower);
		}
		static float EaseInOutSquare(float _fStart, float _fEnd, float _fRatio, float _fPower)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			float fEasedRatio = EaseInOutSquare(_fRatio, _fPower);

			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * fEasedRatio;

			return fInterpolatedValue;
		}
		static Vector3 EaseInOutSquare(Vector3 _vStart, Vector3 _vEnd, float _fRatio, float _fPower)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			float fEasedRatio = EaseInOutSquare(_fRatio, _fPower);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseInOutSquare(Vector4 _vStart, Vector4 _vEnd, float _fRatio, float _fPower)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));
			float fEasedRatio = EaseInOutSquare(_fRatio, _fPower);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		////////////////
		/* EaseInCirc */
		////////////////
		static float EaseInCirc(float _fRatio)
		{
			return 1.f - sqrt(1.f - _fRatio * _fRatio);
		}
		static float EaseInCirc(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * EaseInCirc(_fRatio);

			return fInterpolatedValue;
		}
		static Vector3 EaseInCirc(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseInCirc(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * EaseInCirc(_fRatio);
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * EaseInCirc(_fRatio);
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * EaseInCirc(_fRatio);

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseInCirc(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseInCirc(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * EaseInCirc(_fRatio);
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * EaseInCirc(_fRatio);
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * EaseInCirc(_fRatio);
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * EaseInCirc(_fRatio);

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		/////////////////
		/* EaseOutCirc */
		/////////////////
		static float EaseOutCirc(float _fRatio)
		{
			return sqrt(1.0f - (_fRatio - 1.0f) * (_fRatio - 1.0f));
		}
		static float EaseOutCirc(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * EaseOutCirc(_fRatio);

			return fInterpolatedValue;
		}
		static Vector3 EaseOutCirc(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseOutCirc(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseOutCirc(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseOutCirc(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		/////////////////
		/* EaseOutCirc */
		/////////////////
		static float EaseInOutCirc(float _fRatio)
		{
			_fRatio *= 2.f;
			if (_fRatio < 1.f)
			{
				return -0.5f * (sqrt(1.f - _fRatio * _fRatio) - 1.f);
			}
			else
			{
				_fRatio -= 2.f;
				return 0.5f * (sqrt(1.f - _fRatio * _fRatio) + 1.f);
			}
		}
		static float EaseInOutCirc(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * EaseInOutCirc(_fRatio);

			return fInterpolatedValue;
		}
		static Vector3 EaseInOutCirc(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseInOutCirc(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseInOutCirc(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseInOutCirc(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		////////////////
		/* EaseInBack */
		////////////////
		static float EaseInBack(float _fRatio)
		{
			const float fC1 = 1.70158f;
			const float fC2 = fC1 * 1.525f;

			return _fRatio * _fRatio * ((fC2 + 1.f) * _fRatio - fC2);
		}
		static float EaseInBack(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * EaseInBack(_fRatio);

			return fInterpolatedValue;
		}
		static Vector3 EaseInBack(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseInBack(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseInBack(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseInBack(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		/////////////////
		/* EaseOutBack */
		/////////////////
		static float EaseOutBack(float _fRatio)
		{
			const float fC1 = 1.70158f;
			const float fC3 = fC1 + 1.f;

			return 1.f + fC3 * powf(_fRatio - 1.f, 3.f) + fC1 * powf(_fRatio - 1.f, 2.f);
		}
		static float EaseOutBack(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseOutBack(_fRatio);
			float fInterpolatedValue = _fStart + (_fEnd - _fStart) * fEasedRatio;

			return fInterpolatedValue;
		}
		static Vector3 EaseOutBack(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseOutBack(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseOutBack(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseOutBack(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		///////////////////
		/* EaseInOutBack */
		///////////////////
		static float EaseInOutBack(float _fRatio)
		{
			const float fC1 = 1.70158f * 1.525f;

			_fRatio *= 2.f;
			if (_fRatio < 1.f)
			{
				return 0.5f * (_fRatio * _fRatio * ((fC1 + 1.f) * _fRatio - fC1));
			}
			else
			{
				_fRatio -= 2.f;
				return 0.5f * (_fRatio * _fRatio * ((fC1 + 1.f) * _fRatio + fC1) + 2.f);
			}
		}
		static float EaseInOutBack(float _fStart, float _fEnd, float _fRatio)
		{
			const float fC1 = 1.70158f * 1.525f;
			_fRatio = max(0.f, min(1.f, _fRatio));

			_fRatio *= 2.f;
			if (_fRatio < 1.f)
			{
				return _fStart + (_fEnd - _fStart) * (0.5f * (_fRatio * _fRatio * ((fC1 + 1.f) * _fRatio - fC1)));
			}
			else
			{
				_fRatio -= 2.f;
				return _fStart + (_fEnd - _fStart) * (0.5f * (_fRatio * _fRatio * ((fC1 + 1.f) * _fRatio + fC1) + 2.f));
			}
		}
		static Vector3 EaseInOutBack(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			const float fC1 = 1.70158f * 1.525f;
			_fRatio = max(0.f, min(1.f, _fRatio));

			_fRatio *= 2.f;
			if (_fRatio < 1.f)
			{
				float fEasedRatio = 0.5f * (_fRatio * _fRatio * ((fC1 + 1.0f) * _fRatio - fC1));
				return Vector3(_vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio,
							   _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio,
							   _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio);
			}
			else
			{
				_fRatio -= 2.f;
				float fEasedRatio = 0.5f * (_fRatio * _fRatio * ((fC1 + 1.0f) * _fRatio + fC1) + 2.0f);
				return Vector3(_vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio,
							   _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio,
							   _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio);
			}
		}
		static Vector4 EaseInOutBack(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			const float fC1 = 1.70158f * 1.525f;
			_fRatio = max(0.f, min(1.f, _fRatio));

			_fRatio *= 2.f;
			if (_fRatio < 1.f)
			{
				float fEasedRatio = 0.5f * (_fRatio * _fRatio * ((fC1 + 1.0f) * _fRatio - fC1));
				return Vector4(_vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio,
					_vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio,
					_vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio,
					_vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio);
			}
			else
			{
				_fRatio -= 2.f;
				float fEasedRatio = 0.5f * (_fRatio * _fRatio * ((fC1 + 1.0f) * _fRatio + fC1) + 2.0f);
				return Vector4(_vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio,
					_vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio,
					_vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio,
					_vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio);
			}
		}

		///////////////////
		/* EaseInElastic */
		///////////////////
		static float EaseInElastic(float _fStart, float _fEnd, float _fRatio)
		{
			const float fC4 = (2.f * XM_PI) / 3.f;
			_fRatio = max(0.f, min(1.f, _fRatio));

			if (0 == _fRatio) { return _fStart; }
			if (1 == _fRatio) { return _fEnd; }

			float fEasedRatio = -powf(2.f, 10.f * _fRatio - 10.f) * sinf((_fRatio * 10.f - 10.75f) * fC4);

			return _fStart + (_fEnd - _fStart) * fEasedRatio;
		}
		static Vector3 EaseInElastic(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			const float fC4 = (2.f * XM_PI) / 3.f;
			_fRatio = max(0.f, min(1.f, _fRatio));

			if (0 == _fRatio) { return _vStart; }
			if (1 == _fRatio) { return _vEnd; }

			float fEasedRatio = -powf(2.f, 10.f * _fRatio - 10.f) * sinf((_fRatio * 10.f - 10.75f) * fC4);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseInElastic(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			const float fC4 = (2.f * XM_PI) / 3.f;
			_fRatio = max(0.f, min(1.f, _fRatio));

			if (0 == _fRatio) { return _vStart; }
			if (1 == _fRatio) { return _vEnd; }

			float fEasedRatio = -powf(2.f, 10.f * _fRatio - 10.f) * sinf((_fRatio * 10.f - 10.75f) * fC4);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}
		
		////////////////////
		/* EaseOutElastic */
		////////////////////
		static float EaseOutElastic(float _fStart, float _fEnd, float _fRatio)
		{
			const float fC4 = (2.f * XM_PI) / 3.f;
			_fRatio = max(0.f, min(1.f, _fRatio));

			if (0 == _fRatio) { return _fStart; }
			if (1 == _fRatio) { return _fEnd; }

			float fEasedRatio = powf(2.f, -10.f * _fRatio) * sinf((_fRatio * 10.f - 0.75f) * fC4) + 1.f;

			return _fStart + (_fEnd - _fStart) * fEasedRatio;
		}
		static Vector3 EaseOutElastic(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			const float fC4 = (2.f * XM_PI) / 3.f;
			_fRatio = max(0.f, min(1.f, _fRatio));

			if (0 == _fRatio) { return _vStart; }
			if (1 == _fRatio) { return _vEnd; }

			float fEasedRatio = powf(2.f, -10.f * _fRatio) * sinf((_fRatio * 10.f - 0.75f) * fC4) + 1.f;

			float finterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float finterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float finterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;

			return Vector3(finterpolatedX, finterpolatedY, finterpolatedZ);
		}
		static Vector4 EaseOutElastic(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			const float fC4 = (2.f * XM_PI) / 3.f;
			_fRatio = max(0.f, min(1.f, _fRatio));

			if (0 == _fRatio) { return _vStart; }
			if (1 == _fRatio) { return _vEnd; }

			float fEasedRatio = powf(2.f, -10.f * _fRatio) * sinf((_fRatio * 10.f - 0.75f) * fC4) + 1.f;

			float finterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float finterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float finterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;
			float finterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio;

			return Vector4(finterpolatedX, finterpolatedY, finterpolatedZ, finterpolatedW);
		}

		//////////////////////
		/* EaseInOutElastic */
		//////////////////////
		static float EaseInOutElastic(float _fStart, float _fEnd, float _fRatio)
		{
			const float fC5 = (2.f * XM_PI) / 4.5f;
			_fRatio = max(0.f, min(1.f, _fRatio * 2.f));

			if (0 == _fRatio) { return _fStart; }
			if (2 == _fRatio) { return _fEnd; }

			if (_fRatio < 1.f)
			{
				float fEasedRatio = -0.5f * powf(2.f, 10.f * _fRatio - 10.f) * sinf((_fRatio * 10.f - 10.75f) * fC5);
				return _fStart + (_fEnd - _fStart) * fEasedRatio;
			}
			else
			{
				_fRatio -= 1.f;
				float fEasedRatio = powf(2.f, -10.f * _fRatio) * sinf((_fRatio * 10.f - 10.75f) * fC5) * 0.5f + 1.f;
				return _fStart + (_fEnd - _fStart) * fEasedRatio;
			}
		}
		static Vector3 EaseInOutElastic(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			const float fC5 = (2.f * XM_PI) / 4.5f;
			_fRatio = max(0.f, min(1.f, _fRatio * 2.f));

			if (0 == _fRatio) { return _vStart; }
			if (2 == _fRatio) { return _vEnd; }

			if (_fRatio < 1.f)
			{
				float fEasedRatio = -0.5f * powf(2.f, 10.f * _fRatio - 10.f) * sinf((_fRatio * 10.f - 10.75f) * fC5);
				return Vector3(_vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio,
							   _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio,
							   _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio);
			}
			else
			{
				_fRatio -= 1.f;
				float fEasedRatio = powf(2.f, -10.f * _fRatio) * sinf((_fRatio * 10.f - 10.75f) * fC5) * 0.5f + 1.f;
				return Vector3(_vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio,
							   _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio,
							   _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio);
			}
		}
		static Vector4 EaseInOutElastic(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			const float fC5 = (2.f * XM_PI) / 4.5f;
			_fRatio = max(0.f, min(1.f, _fRatio * 2.f));

			if (0 == _fRatio) { return _vStart; }
			if (2 == _fRatio) { return _vEnd; }

			if (_fRatio < 1.f)
			{
				float fEasedRatio = -0.5f * powf(2.f, 10.f * _fRatio - 10.f) * sinf((_fRatio * 10.f - 10.75f) * fC5);
				return Vector4(_vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio,
					_vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio,
					_vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio,
					_vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio);
			}
			else
			{
				_fRatio -= 1.f;
				float fEasedRatio = powf(2.f, -10.f * _fRatio) * sinf((_fRatio * 10.f - 10.75f) * fC5) * 0.5f + 1.f;
				return Vector4(_vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio,
					_vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio,
					_vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio,
					_vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio);
			}
		}

		//////////////////
		/* EaseInBounce */
		//////////////////
		static float EaseInBounce(float _fRatio)
		{
			return 1.f - EaseOutBounce(1.f - _fRatio);
		}
		static float EaseOutBounce(float _fRatio)
		{
			if (_fRatio < (1.f / 2.75f))
			{
				return 7.5625f * _fRatio * _fRatio;
			}
			else if (_fRatio < (2.f / 2.75f))
			{
				_fRatio -= (1.5f / 2.75f);
				return 7.5625f * _fRatio * _fRatio + 0.75f;
			}
			else if (_fRatio < (2.5f / 2.75f))
			{
				_fRatio -= (2.25f / 2.75f);
				return 7.5625f * _fRatio * _fRatio + 0.9375f;
			}
			else
			{
				_fRatio -= (2.625f / 2.75f);
				return 7.5625f * _fRatio * _fRatio + 0.984375f;
			}
		}
		static float EaseInBounce(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			return _fStart + (_fEnd - _fStart) * EaseInBounce(_fRatio);
		}
		static Vector3 EaseInBounce(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseInBounce(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseInBounce(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseInBounce(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		///////////////////
		/* EaseOutBounce */
		///////////////////
		static float EaseOutBounce(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			return _fStart + (_fEnd - _fStart) * EaseOutBounce(_fRatio);
		}
		static Vector3 EaseOutBounce(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseOutBounce(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;

			return Vector3(fInterpolatedX, fInterpolatedY, fInterpolatedZ);
		}
		static Vector4 EaseOutBounce(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio));

			float fEasedRatio = EaseOutBounce(_fRatio);

			float fInterpolatedX = _vStart.x + (_vEnd.x - _vStart.x) * fEasedRatio;
			float fInterpolatedY = _vStart.y + (_vEnd.y - _vStart.y) * fEasedRatio;
			float fInterpolatedZ = _vStart.z + (_vEnd.z - _vStart.z) * fEasedRatio;
			float fInterpolatedW = _vStart.w + (_vEnd.w - _vStart.w) * fEasedRatio;

			return Vector4(fInterpolatedX, fInterpolatedY, fInterpolatedZ, fInterpolatedW);
		}

		/////////////////////
		/* EaseInOutBounce */
		/////////////////////
		static float EaseInOutBounce(float _fRatio)
		{
			if (_fRatio < 0.5f)
				return 0.5f * EaseInBounce(_fRatio * 2.f);
			else
				return 0.5f * EaseOutBounce(_fRatio * 2.f - 1.f) + 0.5f;
		}
		static float EaseInOutBounce(float _fStart, float _fEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio * 2.f));

			if (_fRatio < 1.f)
			{
				return 0.5f * EaseInBounce(_fStart, _fEnd, _fRatio);
			}
			else
			{
				_fRatio -= 1.f;
				return 0.5f * EaseOutBounce(_fStart, _fEnd, _fRatio) + 0.5f;
			}
		}
		static Vector3 EaseInOutBounce(Vector3 _vStart, Vector3 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio * 2.f));

			if (_fRatio < 1.f)
			{
				return 0.5f * EaseInBounce(_vStart, _vEnd, _fRatio);
			}
			else
			{
				_fRatio -= 1.f;
				return 0.5f * EaseOutBounce(_vStart, _vEnd, _fRatio);
			}
		}
		static Vector4 EaseInOutBounce(Vector4 _vStart, Vector4 _vEnd, float _fRatio)
		{
			_fRatio = max(0.f, min(1.f, _fRatio * 2.f));

			if (_fRatio < 1.f)
			{
				return 0.5f * EaseInBounce(_vStart, _vEnd, _fRatio);
			}
			else
			{
				_fRatio -= 1.f;
				return 0.5f * EaseOutBounce(_vStart, _vEnd, _fRatio);
			}
		}
	};
}
