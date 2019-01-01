#include "Actor/Geometry/GeometrySoup.h"
#include "FileIO/SDL/InputPacket.h"

#include <iostream>

namespace ph
{

GeometrySoup::GeometrySoup() : 
	Geometry(),
	m_geometries()
{}

void GeometrySoup::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	for(const auto& geometry : m_geometries)
	{
		geometry->genPrimitive(data, out_primitives);
	}
}

std::shared_ptr<Geometry> GeometrySoup::genTransformed(const StaticAffineTransform& transform) const
{
	auto tGeometrySoup = std::make_shared<GeometrySoup>();
	for(const auto& geometry : m_geometries)
	{
		const auto& tGeometry = geometry->genTransformed(transform);
		if(!tGeometry)
		{
			std::cerr << "warning: at GeometrySoup::genTransformApplied(), "
			          << "a geometry cannot apply specified transform" << std::endl;
			continue;
		}

		tGeometrySoup->m_geometries.push_back(tGeometry);
	}

	return tGeometrySoup;
}

void GeometrySoup::addGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometries.push_back(geometry);
}

// command interface

namespace
{
	ExitStatus add_geometry(
		const std::shared_ptr<GeometrySoup>& soup,
		const InputPacket& packet)
	{
		soup->addGeometry(packet.get<Geometry>("geometry"));

		return ExitStatus::SUCCESS();
	}
}

SdlTypeInfo GeometrySoup::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "geometry-soup");
}

void GeometrySoup::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<GeometrySoup>();
	}));

	SdlExecutor addGeometry;
	addGeometry.setName("add");
	addGeometry.setFunc<GeometrySoup>(add_geometry);
	cmdRegister.addExecutor(addGeometry);
}

}// end namespace ph