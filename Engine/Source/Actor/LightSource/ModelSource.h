#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Math/math_fwd.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"

#include <memory>
#include <string>

namespace ph
{

class Image;

class ModelSource final : public LightSource, public TCommandInterface<ModelSource>
{
public:
	ModelSource(const Vector3R& emittedRgbRadiance);
	ModelSource(const Path& imagePath);
	ModelSource(const std::shared_ptr<Image>& emittedRadiance);
	virtual ~ModelSource() override;

	virtual std::unique_ptr<Emitter> genEmitter(
		CookingContext& context, EmitterBuildingMaterial&& data) const override;

	virtual std::shared_ptr<Geometry> genGeometry(CookingContext& context) const override;

	virtual std::shared_ptr<Material> genMaterial(CookingContext& context) const override;

	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setBackFaceEmit(bool isBackFaceEmit);

private:
	std::shared_ptr<Image>    m_emittedRadiance;
	std::shared_ptr<Geometry> m_geometry;
	std::shared_ptr<Material> m_material;
	bool                      m_isBackFaceEmit;

	static const Logger logger;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<ModelSource> ciLoad(const InputPacket& packet);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  light-source              </category>
	<type_name> model                     </type_name>
	<extend>    light-source.light-source </extend>

	<name> Model Source </name>
	<description>
		A light source that emits energy from the surface of a geometry. A surface 
		material model can also be given to describe its surface appearance.
	</description>

	<command type="creator">
		<input name="geometry" type="geometry">
			<description>A geometry that defines the surface energy is going to emit from.</description>
		</input>
		<input name="material" type="material">
			<description>A material that describes this source's surface appearance.</description>
		</input>
		<input name="emitted-radiance" type="image">
			<description>An image that describes the emitted radiance across the surface.</description>
		</input>
		<input name="emitted-radiance" type="vector3">
			<description>Specify a constant emitted radiance across the surface in linear-SRGB.</description>
		</input>
		<input name="emit-mode" type="string">
			<description>
				Selects the side where the energy is allowed to emit. It can be "front" or "back".
			</description>
		</input>
	</command>

	</SDL_interface>
*/