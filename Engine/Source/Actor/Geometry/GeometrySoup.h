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
		const PrimitiveBuildingMaterial&         data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

	void add(const std::shared_ptr<Geometry>& geometry);

	bool addTransformed(
		const std::shared_ptr<Geometry>& geometry, 
		const math::StaticAffineTransform& transform);

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

	<command type="executor" name="add-transformed">
		<description>
			Applies transformations on a geometry then add it to the soup.
		</description>
		<input name="geometry" type="geometry">
			<description>The geometry to be added.</description>
		</input>
		<input name="translation" type="vector3">
			<description>Offset amount.</description>
		</input>
		<input name="rotation-axis" type="vector3">
			<description>Axis of rotation.</description>
		</input>
		<input name="rotation-degrees" type="real">
			<description>Amount of rotation along the rotation axis in degrees.</description>
		</input>
		<input name="scale" type="vector3">
			<description>Magnify/minify factor to be applied on the geometry.</description>
		</input>
	</command>

	</SDL_interface>
*/
