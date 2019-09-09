#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"

#include <memory>

namespace ph
{

class BinaryMixedSurfaceMaterial final : 

	public SurfaceMaterial, 
	public TCommandInterface<BinaryMixedSurfaceMaterial>
{
public:
	enum class EMode
	{
		LERP
	};

	BinaryMixedSurfaceMaterial();

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

	void setMode(EMode mode);
	void setMaterials(
		const std::shared_ptr<SurfaceMaterial>& material0,
		const std::shared_ptr<SurfaceMaterial>& material1);
	void setFactor(real factor);
	void setFactor(const std::shared_ptr<Image>& factor);

private:
	EMode                            m_mode;
	std::shared_ptr<SurfaceMaterial> m_material0;
	std::shared_ptr<SurfaceMaterial> m_material1;
	std::shared_ptr<Image>           m_factor;

// command interface
public:
	explicit BinaryMixedSurfaceMaterial(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  material             </category>
	<type_name> binary-mixed-surface </type_name>
	<extend>    material.material    </extend>

	<name> Binary Mixed Surface </name>
	<description>
		Mixing two surface materials in various ways.
	</description>

	<command type="creator">
		<input name="mode" type="string">
			<description>
				Specifying how two materials are mixed. The only mode supported
				now is "lerp".
			</description>
		</input>
		<input name="factor" type="real">
			<description>
				A number in [0, 1] controlling the contribution from each material.
			</description>
		</input>
		<input name="factor" type="image">
			<description>
				An image controlling the contribution from each material.
			</description>
		</input>
		<input name="material-0" type="material">
			<description>
				The material that participates the mixing process.
			</description>
		</input>
		<input name="material-1" type="material">
			<description>
				The material that participates the mixing process.
			</description>
		</input>
	</command>

	</SDL_interface>
*/