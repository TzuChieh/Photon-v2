#include "Quaternion.h"
#include "Vector3f.h"
#include "Matrix4f.h"

namespace ph
{

Quaternion::Quaternion(const Vector3f& normalizedAxis, const float32 angle)
{
	setRot(normalizedAxis, angle);
}

Quaternion Quaternion::mul(const Vector3f& xyz) const
{
	return Quaternion(w * xyz.x + y * xyz.z - z * xyz.y,
		w * xyz.y - x * xyz.z + z * xyz.x,
		w * xyz.z + x * xyz.y - y * xyz.x,
		-x * xyz.x - y * xyz.y - z * xyz.z);
}

void Quaternion::mul(const Vector3f& xyz, Quaternion* out_result) const
{
	out_result->x = w * xyz.x + y * xyz.z - z * xyz.y;
	out_result->y = w * xyz.y - x * xyz.z + z * xyz.x;
	out_result->z = w * xyz.z + x * xyz.y - y * xyz.x;
	out_result->w = -x * xyz.x - y * xyz.y - z * xyz.z;
}

void Quaternion::setRot(const Vector3f& normalizedAxis, const float32 angle)
{
	float32 sinHalfAngle = sin(angle / 2.0f);
	float32 cosHalfAngle = cos(angle / 2.0f);

	x = normalizedAxis.x * sinHalfAngle;
	y = normalizedAxis.y * sinHalfAngle;
	z = normalizedAxis.z * sinHalfAngle;
	w = cosHalfAngle;
}

void Quaternion::toRotationMatrix(Matrix4f* result) const
{
	result->m[0][0] = 1.0f - 2.0f*(y*y + z*z);
	result->m[0][1] = 2.0f*(x*y - z*w);
	result->m[0][2] = 2.0f*(y*w + x*z);
	result->m[0][3] = 0.0f;

	result->m[1][0] = 2.0f*(x*y + z*w);
	result->m[1][1] = 1.0f - 2.0f*(x*x + z*z);
	result->m[1][2] = 2.0f*(y*z - x*w);
	result->m[1][3] = 0.0f;

	result->m[2][0] = 2.0f*(x*z - y*w);
	result->m[2][1] = 2.0f*(y*z + x*w);
	result->m[2][2] = 1.0f - 2.0f*(x*x + y*y);
	result->m[2][3] = 0.0f;

	result->m[3][0] = 0.0f;
	result->m[3][1] = 0.0f;
	result->m[3][2] = 0.0f;
	result->m[3][3] = 1.0f;
}

Quaternion Quaternion::nlerp(const Quaternion& dest, float32 lerpFactor, bool shortest) const
{
	Quaternion correctedDest = dest;

	if(shortest && (*this).dot(dest) < 0)
		correctedDest.set(dest.mul(-1.0f));

	return correctedDest.sub(*this).mul(lerpFactor).add(*this).normalize();
}

Quaternion Quaternion::slerp(const Quaternion& dest, float32 lerpFactor, bool shortest) const
{
	const float32 EPSILON = 1e3f;

	float32 cosValue = (*this).dot(dest);
	Quaternion correctedDest = dest;

	if(shortest && cosValue < 0)
	{
		cosValue = -cosValue;
		correctedDest.set(dest.mul(-1.0f));
	}

	if(abs(cosValue) >= 1 - EPSILON)
		return nlerp(correctedDest, lerpFactor, false);

	float32 sinValue = sqrt(1.0f - cosValue * cosValue);
	float32 angle = atan2(sinValue, cosValue);
	float32 invSin = 1.0f / sinValue;

	float32 srcFactor = sin((1.0f - lerpFactor) * angle) * invSin;
	float32 destFactor = sin((lerpFactor)* angle) * invSin;

	return (*this).mul(srcFactor).add(correctedDest.mul(destFactor));
}

}// end namespace ph