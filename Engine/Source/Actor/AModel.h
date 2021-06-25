#pragma once

#include "Math/Transform/StaticAffineTransform.h"
#include "Actor/PhysicalActor.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;
class MotionSource;

class AModel : public PhysicalActor
{
public:
	AModel();
	AModel(const std::shared_ptr<Geometry>& geometry, 
	       const std::shared_ptr<Material>& material);
	AModel(const AModel& other);

	CookedUnit cook(CookingContext& context) override;

	AModel& operator = (AModel rhs);

	const Geometry* getGeometry() const;
	const Material* getMaterial() const;
	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setMotionSource(const std::shared_ptr<MotionSource>& motion);

	friend void swap(AModel& first, AModel& second);

private:
	std::shared_ptr<Geometry>     m_geometry;
	std::shared_ptr<Material>     m_material;
	std::shared_ptr<MotionSource> m_motionSource;
};

}// end namespace ph
