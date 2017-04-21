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
	virtual ~GTriangleMesh() override;

	virtual void discretize(const PrimitiveBuildingMaterial& data,
	                        std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	void addTriangle(const GTriangle gTriangle);

private:
	std::vector<GTriangle> m_gTriangles;

// command interface
public:
	GTriangleMesh(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<GTriangleMesh>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph