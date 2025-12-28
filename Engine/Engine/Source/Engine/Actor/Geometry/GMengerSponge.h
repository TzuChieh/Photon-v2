#pragma once

#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/SDL/sdl_interface.h"

#include <Common/primitive_type.h>

namespace ph
{

class GCuboid;

class GMengerSponge final : public Geometry
{
public:
	GMengerSponge();
	explicit GMengerSponge(uint32 numIteration);

	void storeCooked(
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const override;

private:
	uint32 m_numIteration;

	void genMengerSpongeRecursive(
		const math::Vector3R& minVertex,
		const math::Vector3R& maxVertex,
		uint32                currentIteration,
		std::vector<GCuboid>& cubes) const;

public:
	PH_DEFINE_SDL_CLASS(GMengerSponge, clazz)
	{
		clazz.typeName("menger-sponge");
		clazz.docName("Menger Sponge Geometry");
		clazz.description("A fractal geometry.");
		clazz.baseOn<Geometry>();

		TSdlInteger<OwnerType, uint32> numIteration("iterations", &OwnerType::m_numIteration);
		numIteration.description("Number of recursive iterations on the fractal surface detail.");
		numIteration.defaultTo(3);
		clazz.addField(numIteration);
	}
};

}// end namespace ph
