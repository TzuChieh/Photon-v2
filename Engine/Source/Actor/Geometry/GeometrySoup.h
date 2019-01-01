#pragma once

#include "Actor/Geometry/Geometry.h"

#include <vector>
#include <memory>

namespace ph
{

class GeometrySoup : public Geometry
{
public:
	GeometrySoup();

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const StaticAffineTransform& transform) const override;

	void addGeometry(const std::shared_ptr<Geometry>& geometry);

private:
	std::vector<std::shared_ptr<Geometry>> m_geometries;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  geometry          </category>
	<type_name> geometry-soup     </type_name>
	<extend>    geometry.geometry </extend>

	<name> Geometry Soup </name>
	<description>
		A collection of geometry.
	</description>

	<command type="creator"/>

	<command type="executor" name="add">
		<description>
			Adds a geometry to the soup.
		</description>
		<input name="geometry" type="geometry">
			<description>The geometry to be added.</description>
		</input>
	</command>

	</SDL_interface>
*/