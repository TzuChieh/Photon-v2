#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Material/Utility/SurfaceLayerInfo.h"

#include <vector>

namespace ph
{

class LayeredSurface : public SurfaceMaterial, public TCommandInterface<LayeredSurface>
{
public:
	LayeredSurface();

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;
	
	void addLayer();
	void setLayer(std::size_t layerIndex, const SurfaceLayerInfo& layer);

private:
	std::vector<SurfaceLayerInfo> m_layers;

// command interface
public:
	explicit LayeredSurface(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  material          </category>
	<type_name> layered-surface   </type_name>
	<extend>    material.material </extend>

	<name> Layered Surface </name>
	<description>
		A material model for surfaces with matte look, such as chalk and moon.
	</description>

	<command type="creator"/>

	<command type="executor" name="add">
		<description>
			Appends a layer to the bottom of the existing layers.
		</description>
	</command>

	<command type="executor" name="set">

		<description>
			Creates a new surface layer and set it to a specified layer index.
			If there are N layers, the top one will have index 0 and the bottom
			one will have index N-1.
		</description>

		<input name="index" type="integer">
			<description>The target layer index.</description>
		</input>
		<input name="roughness" type="real">
			<description>
				Isotropic surface roughness in [0, 1], the material will appear
				to be smoother with smaller roughness value.
			</description>
		</input>
		<input name="ior-n" type="vector3">
			<description>
				The real part of the layer's index of refraction in linear-SRGB.
			</description>
		</input>
		<input name="ior-k" type="vector3">
			<description>
				The imaginary part of the layer's index of refraction in linear-SRGB.
			</description>
		</input>
		<input name="ior-n" type="real">
			<description>
				The real part of the layer's index of refraction as a raw constant.
			</description>
		</input>
		<input name="ior-k" type="real">
			<description>
				The imaginary part of the layer's index of refraction as a raw constant.
			</description>
		</input>
		<input name="depth" type="real">
			<description>
				Thickness of the layer.
			</description>
		</input>
		<input name="g" type="real">
			<description>
				The g variable in Henyey-Greenstein phase function.
			</description>
		</input>
		<input name="sigma-a" type="real">
			<description>
				The volume absorption coefficient.
			</description>
		</input>
		<input name="sigma-s" type="real">
			<description>
				The volume scattering coefficient.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
