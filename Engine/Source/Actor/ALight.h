#pragma once

#include "Math/Transform/StaticAffineTransform.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/PhysicalActor.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Common/Logger.h"
#include "Math/Transform/StaticRigidTransform.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;
class LightSource;
class Transform;

class ALight final : public PhysicalActor, public TCommandInterface<ALight>
{
public:
	ALight();
	ALight(const std::shared_ptr<LightSource>& lightSource);
	ALight(const ALight& other);
	virtual ~ALight() override;

	virtual CookedUnit cook(CookingContext& context) const override;

	const LightSource* getLightSource() const;
	void setLightSource(const std::shared_ptr<LightSource>& lightSource);

	ALight& operator = (ALight rhs);

	friend void swap(ALight& first, ALight& second);

private:
	std::shared_ptr<LightSource> m_lightSource;

	CookedUnit buildGeometricLight(
		CookingContext&                  context, 
		std::shared_ptr<Geometry> geometry,
		std::shared_ptr<Material> material) const;

	// Tries to return a geometry suitable for emitter calculations (can be the 
	// original one if it is already suitable). If the current actor has undesired 
	// configurations, nullptr is returned.
	std::shared_ptr<Geometry> getSanifiedGeometry(
		CookingContext&                  context,
		const std::shared_ptr<Geometry>& geometry,
		std::unique_ptr<RigidTransform>* out_baseLW,
		std::unique_ptr<RigidTransform>* out_baseWL) const;

	static const Logger logger;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<ALight> ciLoad(const InputPacket& packet);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  actor          </category>
	<type_name> light          </type_name>
	<extend>    actor.physical </extend>

	<name> Light Actor </name>
	<description>
		An actor that represents a light in the scene.
	</description>

	<command type="creator">
		<input name="light-source" type="light-source">
			<description>The source of the energy.</description>
		</input>
	</command>

	</SDL_interface>
*/