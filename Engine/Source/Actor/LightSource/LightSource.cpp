#include "Actor/LightSource/LightSource.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/MatteOpaque.h"

namespace ph
{

std::shared_ptr<Geometry> LightSource::genGeometry(CookingContext& ctx) const
{
	return nullptr;
}

std::shared_ptr<Material> LightSource::genMaterial(CookingContext& ctx) const
{
	const math::Vector3R linearSrgbAlbedo(0.5_r);
	return std::make_shared<MatteOpaque>(linearSrgbAlbedo);
}

}// end namespace ph
