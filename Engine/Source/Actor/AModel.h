#pragma once

#include "Math/Transform/StaticAffineTransform.h"
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

class AModel : public PhysicalActor, public TCommandInterface<AModel>
{
public:
	AModel();
	AModel(const std::shared_ptr<Geometry>& geometry, 
	       const std::shared_ptr<Material>& material);
	AModel(const AModel& other);

	CookedUnit cook(CookingContext& context) const override;

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
	explicit AModel(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  actor          </category>
	<type_name> model          </type_name>
	<extend>    actor.physical </extend>

	<name> Model Actor </name>
	<description>
		An actor that has a certain 3-D shape in the scene.
	</description>

	<command type="creator">
		<input name="geometry" type="geometry">
			<description>A geometry that represent this actor's shape.</description>
		</input>
		<input name="material" type="material">
			<description>A material that describes this actor's surface appearance.</description>
		</input>
	</command>

	</SDL_interface>
*/