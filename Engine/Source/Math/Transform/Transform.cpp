#include "Math/Transform/Transform.h"
#include "Core/Time.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/TPoint3.h"
#include "Math/TVector3.h"
#include "Math/TPoint3.h"

namespace ph
{

Transform::~Transform() = default;

void Transform::transform(const Vector3R& vector, Vector3R* const out_vector) const
{
	transformVector(vector, Time(), out_vector);
}

void Transform::transform(const Point3R& point, Point3R* const out_point) const
{
	Vector3R result;
	transformPoint(Vector3R(point), Time(), &result);
	*out_point = Point3R(result);

	//*out_point = Point3R(Vector3R(0));
}

void Transform::transform(const Ray& ray, Ray* const out_ray) const
{
	real rayMinT, rayMaxT;
	transformLineSegment(ray.getOrigin(), ray.getDirection(), 
	                     ray.getMinT(), ray.getMaxT(), 
	                     Time(),
	                     &(out_ray->getOrigin()), &(out_ray->getDirection()), 
	                     &rayMinT, &rayMaxT);
	out_ray->setMinT(rayMinT);
	out_ray->setMaxT(rayMaxT);
}

}// end namespace ph