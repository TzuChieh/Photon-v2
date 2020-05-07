#include "Core/Receiver/PerspectiveReceiver.h"
#include "Core/Ray.h"
#include "Core/Sample.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "DataIO/SDL/InputPacket.h"
#include "Math/math.h"
#include "DataIO/SDL/InputPrototype.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

void PerspectiveReceiver::updateTransforms()
{
	const math::Vector2D fResolution(getRasterResolution());
	const float64 sensorHeight = m_sensorWidth / getAspectRatio();

	PH_ASSERT_GT(m_sensorWidth, 0);
	PH_ASSERT_GT(sensorHeight, 0);

	m_rasterToReceiverDecomposed = math::TDecomposedTransform<float64>();
	m_rasterToReceiverDecomposed.scale(
		-m_sensorWidth / fResolution.x,
		-sensorHeight / fResolution.y,
		1);
	m_rasterToReceiverDecomposed.translate(
		m_sensorWidth / 2,
		sensorHeight / 2,
		m_sensorOffset);

	m_rasterToReceiver = std::make_shared<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(m_rasterToReceiverDecomposed));
	m_receiverToWorld  = std::make_shared<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(m_receiverToWorldDecomposed));
}

// command interface

PerspectiveReceiver::PerspectiveReceiver(const InputPacket& packet) :
	Receiver(packet)
{
	const std::string FOV_DEGREE_VAR       = "fov-degree";
	const std::string SENSOR_WIDTH_MM_VAR  = "sensor-width-mm";
	const std::string SENSOR_OFFSET_MM_VAR = "sensor-offset-mm";

	InputPrototype fovBasedInput;
	fovBasedInput.addReal(FOV_DEGREE_VAR);

	InputPrototype dimensionalInput;
	dimensionalInput.addReal(SENSOR_WIDTH_MM_VAR);
	dimensionalInput.addReal(SENSOR_OFFSET_MM_VAR);

	real sensorWidthMM  = 36.0_r;
	real sensorOffsetMM = 36.0_r;
	if(packet.isPrototypeMatched(fovBasedInput))
	{
		// Respect sensor dimensions; modify sensor offset to satisfy FoV requirement
		const auto fovDegree = packet.getReal(FOV_DEGREE_VAR);
		const auto halfFov   = math::to_radians(fovDegree) * 0.5_r;
		sensorWidthMM  = packet.getReal(SENSOR_WIDTH_MM_VAR, sensorWidthMM);
		sensorOffsetMM = (sensorOffsetMM * 0.5_r) / std::tan(halfFov);
	}
	else if(packet.isPrototypeMatched(dimensionalInput))
	{
		sensorWidthMM  = packet.getReal(SENSOR_WIDTH_MM_VAR);
		sensorOffsetMM = packet.getReal(SENSOR_OFFSET_MM_VAR);
	}
	else
	{
		std::cerr << "warning: in PerspectiveReceiver::PerspectiveReceiver(), bad input format" << std::endl;
	}

	m_sensorWidth  = sensorWidthMM / 1000.0;
	m_sensorOffset = sensorOffsetMM / 1000.0;

	updateTransforms();
}

SdlTypeInfo PerspectiveReceiver::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RECEIVER, "perspective");
}

void PerspectiveReceiver::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
