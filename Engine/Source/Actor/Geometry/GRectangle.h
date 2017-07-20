#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class GTriangleMesh;

class GRectangle : public Geometry, public TCommandInterface<GRectangle>
{
public:
	GRectangle(const real width, const real height);
	virtual ~GRectangle() override;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data,
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;
	virtual std::shared_ptr<Geometry> genTransformApplied(const StaticTransform& transform) const override;

private:
	real m_width;
	real m_height;

	std::shared_ptr<GTriangleMesh> genTriangleMesh() const;

	static bool checkData(const PrimitiveBuildingMaterial& data, const real width, const real height);

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<GRectangle> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<GRectangle>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph