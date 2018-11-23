#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Math/math_fwd.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Actor/Image/Image.h"

#include <memory>

namespace ph
{

class MatteOpaque : public SurfaceMaterial, public TCommandInterface<MatteOpaque>
{
public:
	MatteOpaque();
	MatteOpaque(const Vector3R& linearSrgbAlbedo);
	~MatteOpaque() override;

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

	void setAlbedo(const Vector3R& albedo);
	void setAlbedo(const real r, const real g, const real b);
	void setAlbedo(const std::shared_ptr<Image>& albedo);

private:
	std::shared_ptr<Image> m_albedo;

// command interface
public:
	explicit MatteOpaque(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  material          </category>
	<type_name> matte-opaque      </type_name>
	<inherit>   material.material </inherit>

	<name> Matte Opaque </name>
	<description>
		A material model for surfaces with matte look, such as chalk and moon.
	</description>

	<command type="creator">

		<input name="albedo" type="real">
			<description>A constant albedo in linear SRGB.</description>
		</input>

		<input name="albedo" type="vector3">
			<description>An albedo value in linear SRGB.</description>
		</input>

	</command>

	</SDL_interface>
*/