#pragma once

#include "Math/Transform/StaticTransform.h"
#include "Actor/PhysicalActor.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;
class MotionSource;

class AModel final : public PhysicalActor, public TCommandInterface<AModel>
{
public:
	AModel();
	AModel(const std::shared_ptr<Geometry>& geometry, 
	       const std::shared_ptr<Material>& material);
	AModel(const AModel& other);
	virtual ~AModel() override;

	virtual void cook(CookedActor* const out_cookedActor) const override;

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

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<AModel> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<AModel>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph