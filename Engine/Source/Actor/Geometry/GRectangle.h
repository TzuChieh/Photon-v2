#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class GRectangle : public Geometry, public TCommandInterface<GRectangle>
{
public:
	GRectangle(const real width, const real height);
	virtual ~GRectangle() override;

	virtual void discretize(const PrimitiveBuildingMaterial& data,
	                        std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

private:
	real m_width;
	real m_height;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real width, const real height);

// command interface
public:
	GRectangle(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<GRectangle>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph