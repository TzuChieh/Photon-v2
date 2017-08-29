#pragma once

#include "Actor/Geometry/GTriangle.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <vector>
#include <memory>

namespace ph
{

class GTriangleMesh final : public Geometry, public TCommandInterface<GTriangleMesh>
{
public:
	GTriangleMesh();
	GTriangleMesh(const std::vector<Vector3R>& positions, 
	              const std::vector<Vector3R>& texCoords, 
	              const std::vector<Vector3R>& normals);
	virtual ~GTriangleMesh() override;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data,
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;
	virtual std::shared_ptr<Geometry> genTransformApplied(const StaticTransform& transform) const override;

	void addTriangle(const GTriangle gTriangle);

private:
	std::vector<GTriangle> m_gTriangles;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<GTriangleMesh> ciLoad(const InputPacket& packet);
};

}// end namespace ph