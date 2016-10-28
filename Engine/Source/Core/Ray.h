#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class Ray final
{
public:
	Ray(const Vector3f& origin, const Vector3f& direction);
	Ray();

	void accumulateLiWeight(const Vector3f& newLiWeight);
	void addLiRadiance(const Vector3f& newLiRadiance);

	const Vector3f& getOrigin() const
	{
		return m_origin;
	}

	const Vector3f& getDirection() const
	{
		return m_direction;
	}

	void getOrigin(Vector3f* const out_origin) const
	{
		m_origin.set(out_origin);
	}

	void getDirection(Vector3f* const out_direction) const
	{
		m_direction.set(out_direction);
	}

	void setOrigin(const Vector3f& origin)
	{
		m_origin.set(origin);
	}

	void setDirection(const Vector3f& direction)
	{
		m_direction.set(direction);
	}

private:
	Vector3f m_origin;
	Vector3f m_direction;
	Vector3f m_LiWeight;
	Vector3f m_LiRadiance;
};

}// end namespace ph