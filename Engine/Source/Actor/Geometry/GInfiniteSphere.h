#pragma once

#include "Actor/Geometry/Geometry.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "Common/primitive_type.h"

namespace ph
{

class GInfiniteSphere : public Geometry, public TCommandInterface<GInfiniteSphere>
{
public:
	explicit GInfiniteSphere(real boundRadius);

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

private:
	real m_boundRadius;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<GInfiniteSphere> ciLoad(const InputPacket& packet);
};

}// end namespace ph