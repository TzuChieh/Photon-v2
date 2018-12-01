#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/math.h"

#include <vector>
#include <string>
#include <utility>

namespace ph
{

class Ray;

template<typename T>
class TAABB3D;

using AABB3D = TAABB3D<real>;

template<typename T>
class TAABB3D
{
public:
	static TAABB3D makeUnioned(const TAABB3D& a, const TAABB3D& b);

public:
	TAABB3D();
	TAABB3D(const TVector3<T>& point);
	TAABB3D(const TVector3<T>& minVertex, const TVector3<T>& maxVertex);

	TAABB3D& unionWith(const TAABB3D& other);
	TAABB3D& unionWith(const TVector3<T>& point);
	void setMinVertex(const TVector3<T>& minVertex);
	void setMaxVertex(const TVector3<T>& maxVertex);
	void expand(const TVector3<T>& amount);

	bool isIntersectingVolume(const Ray& ray) const;
	bool isIntersectingVolume(const Ray& ray, real* out_rayNearHitT, real* out_rayFarHitT) const;
	bool isIntersectingVolume(const TAABB3D& other) const;
	bool isPoint() const;
	bool isFiniteVolume() const;

	// FIXME: too slow
	std::vector<TVector3<T>> getVertices() const;

	void getMinMaxVertices(TVector3<T>* out_minVertex, TVector3<T>* out_maxVertex) const;
	const TVector3<T>& getMinVertex() const;
	const TVector3<T>& getMaxVertex() const;
	Vector3R getCentroid() const;
	TVector3<T> getExtents() const;
	T getSurfaceArea() const;
	T getVolume() const;
	std::pair<TAABB3D, TAABB3D> getSplitted(int axis, T splitPoint) const;

	std::string toString() const;

private:
	TVector3<T> m_minVertex;
	TVector3<T> m_maxVertex;
};

}// end namespace ph

#include "Core/Bound/TAABB3D.ipp"