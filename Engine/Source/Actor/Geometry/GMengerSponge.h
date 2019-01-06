#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"

namespace ph
{

class GCuboid;

class GMengerSponge final : public Geometry, public TCommandInterface<GMengerSponge>
{
public:
	GMengerSponge();
	explicit GMengerSponge(uint32 numIteration);

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

private:
	uint32 m_numIteration;

	void genMengerSpongeRecursive(
		const Vector3R&       minVertex,
		const Vector3R&       maxVertex,
		uint32                currentIteration,
		std::vector<GCuboid>& cubes) const;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph