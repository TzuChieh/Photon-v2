#include "Actor/Geometry/GeometrySoup.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/Transform/TDecomposedTransform.h"

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

std::shared_ptr<Geometry> GeometrySoup::genTransformed(
	const math::StaticAffineTransform& transform) const
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

void GeometrySoup::add(const std::shared_ptr<Geometry>& geometry)
{
	m_geometries.push_back(geometry);
}

bool GeometrySoup::addTransformed(
	const std::shared_ptr<Geometry>& geometry,
	const math::StaticAffineTransform& transform)
{
	const auto& transformed = geometry->genTransformed(transform);
	if(!transformed)
	{
		return false;
	}

	add(transformed);
	return true;
}

// command interface

namespace
{
	ExitStatus add_geometry(
		const std::shared_ptr<GeometrySoup>& soup,
		const InputPacket& packet)
	{
		const auto& geometry = packet.get<Geometry>("geometry", DataTreatment::REQUIRED());
		if(!geometry)
		{
			return ExitStatus::BAD_INPUT();
		}

		soup->add(geometry);

		return ExitStatus::SUCCESS();
	}

	ExitStatus add_transformed_geometry(
		const std::shared_ptr<GeometrySoup>& soup,
		const InputPacket& packet)
	{
		const auto& geometry = packet.get<Geometry>("geometry", DataTreatment::REQUIRED());
		if(!geometry)
		{
			return ExitStatus::BAD_INPUT();
		}

		const auto translation     = packet.getVector3("translation",      math::Vector3R(0));
		const auto rotationAxis    = packet.getVector3("rotation-axis",    math::Vector3R(0, 1, 0));
		const auto rotationDegrees = packet.getReal   ("rotation-degrees", 0.0_r);
		const auto scale           = packet.getVector3("scale",            math::Vector3R(1));

		math::TDecomposedTransform<real> transform;
		transform.translate(translation);
		transform.rotate(rotationAxis, rotationDegrees);
		transform.scale(scale);

		if(soup->addTransformed(geometry, math::StaticAffineTransform::makeForward(transform)))
		{
			return ExitStatus::SUCCESS();
		}
		else
		{
			return ExitStatus::FAILURE(
				"input geometry cannot be transformed natively, not adding");
		}
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

	SdlExecutor addTransformedGeometry;
	addTransformedGeometry.setName("add-transformed");
	addTransformedGeometry.setFunc<GeometrySoup>(add_transformed_geometry);
	cmdRegister.addExecutor(addTransformedGeometry);
}

}// end namespace ph
