#include "FFCore/Math/Vec3.h"

#include "FFCore/Core/HString.h"
#include "FFCore/Math/MathCore.h"

namespace FF
{
	Vec3 Vec3::Zero = Vec3(0, 0, 0);
	
	float Vec3::Dot(const Vec3& lhs, const Vec3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	Vec3 Vec3::Cross(const Vec3& lhs, const Vec3& rhs)
	{
		return
		{
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	float Vec3::Angle(const Vec3& lhs, const Vec3& rhs)
	{
		float sqMagL = lhs.LenSq();
		float sqMagR = rhs.LenSq();

		if (sqMagL < MathCore::FloatEpsilon || sqMagR < MathCore::FloatEpsilon)
		{
			return 0.0f;
		}

		float dot = Dot(lhs, rhs);
		float len = sqrtf(sqMagL) * sqrtf(sqMagR);
		return acosf(dot / len);
	}

	void Vec3::Normalize()
	{
		float lenSq = x * x + y * y + z * z;
		if (lenSq < MathCore::FloatEpsilon)
		{
			return;
		}
		float invLen = 1.0f / sqrtf(lenSq);

		x *= invLen;
		y *= invLen;
		z *= invLen;
	}

	Vec3 Vec3::Normalized() const
	{
		float lenSq = x * x + y * y + z * z;

		if (lenSq < MathCore::FloatEpsilon)
		{
			return Vec3(x, y, z);
		}
		float invLen = 1.0f / sqrtf(lenSq);

		return Vec3(
			x * invLen,
			y * invLen,
			z * invLen
		);
	}

	HString Vec3::ToString() const
	{
		char buffer[128];
		sprintf_s(buffer, "X: %.2f Y: %.2f Z: %.2f", x, y, z);
		return buffer;
	}

	float Vec3::LenSq() const
	{
		return x * x + y * y + z * z;
	}

	float Vec3::Len() const
	{
		float lenSq = LenSq();
		if (lenSq < MathCore::FloatEpsilon)
		{
			return 0.0f;
		}
		return sqrtf(lenSq);
	}

	Vec3 Vec3::Project(const Vec3& a, const Vec3& b)
	{
		float magBSq = b.Len();
		if (magBSq < MathCore::FloatEpsilon)
		{
			return Vec3();
		}
		float scale = Dot(a, b) / magBSq;
		return b * scale;
	}

	Vec3 Vec3::Reject(const Vec3& a, const Vec3& b)
	{
		Vec3 projection = Project(a, b);
		return a - projection;
	}

	Vec3 Vec3::Reflect(const Vec3& a, const Vec3& b)
	{
		float magBSq = b.Len();
		if (magBSq < MathCore::FloatEpsilon)
		{
			return Vec3();
		}
		float scale = Dot(a, b) / magBSq;
		Vec3 proj2 = b * (scale * 2);
		return a - proj2;
	}

	float Vec3::Distance(const Vec3& a, const Vec3& b)
	{
		return std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2) + std::pow(b.z - a.z, 2));
	}

	float Vec3::DistanceSq(const Vec3& a, const Vec3& b)
	{
		return std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2) + std::pow(b.z - a.z, 2);
	}

	Vec3 Vec3::Lerp(const Vec3& a, const Vec3& b, float alpha)
	{
		return 
		{
			MathCore::Lerp(a.x, b.x, alpha),
			MathCore::Lerp(a.y, b.y, alpha),
			MathCore::Lerp(a.z, b.z, alpha)
		};
	}

	Vec3& operator+=(Vec3& lhs, Vec3 rhs)
	{
		lhs.x = lhs.x + rhs.x;
		lhs.y = lhs.y + rhs.y;
		lhs.z = lhs.z + rhs.z;
		return lhs;
	}

	Vec3& operator-=(Vec3& lhs, Vec3 rhs)
	{
		lhs.x = lhs.x - rhs.x;
		lhs.y = lhs.y - rhs.y;
		lhs.z = lhs.z - rhs.z;
		return lhs;
	}

	Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}

	Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	Vec3 operator*(const Vec3& vec, float num)
	{
		return { vec.x * num, vec.y * num, vec.z * num };
	}

	bool operator==(const Vec3& lhs, const Vec3& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	bool operator!=(const Vec3& lhs, const Vec3& rhs)
	{
		return !(lhs == rhs);
	}
}

#undef VEC3_EPSILON