#pragma once

#include "Math/TransformInfo.h"
#include "Math/Transform.h"
#include "Actor/PhysicalActor.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;

class AModel final : public PhysicalActor
{
public:
	AModel();
	AModel(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material);
	AModel(const AModel& other);
	AModel(const InputPacket& packet);
	virtual ~AModel() override;

	virtual void cook(CookedActor* const out_cookedActor) const override;

	AModel& operator = (AModel rhs);

	const Geometry* getGeometry() const;
	const Material* getMaterial() const;
	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);

	friend void swap(AModel& first, AModel& second);

private:
	std::shared_ptr<Geometry> m_geometry;
	std::shared_ptr<Material> m_material;
};

}// end namespace ph