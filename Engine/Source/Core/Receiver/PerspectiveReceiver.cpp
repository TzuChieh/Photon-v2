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

	// The real width and height of the sensor (in millimeters)
	const float64 sensorWidthMM  = m_sensorWidthMM;
	const float64 sensorHeightMM = sensorWidthMM / getAspectRatio();
	PH_ASSERT_GT(sensorWidthMM,  0);
	PH_ASSERT_GT(sensorHeightMM, 0);

	m_rasterToReceiverDecomposed = math::TDecomposedTransform<float64>();
	m_rasterToReceiverDecomposed.scale(
		-sensorWidthMM / fResolution.x,
		-sensorHeightMM / fResolution.y,
		1);
	m_rasterToReceiverDecomposed.translate(
		sensorWidthMM / 2, 
		sensorHeightMM / 2, 
		m_sensorOffsetMM);

	m_rasterToReceiver = std::make_shared<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(m_rasterToReceiverDecomposed));
	m_receiverToWorld  = std::make_shared<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(m_receiverToWorldDecomposed));
}

// command interface

PerspectiveReceiver::PerspectiveReceiver(const InputPacket& packet) :

	Receiver(packet),

	m_sensorWidthMM(36.0_r), m_sensorOffsetMM(36.0_r)
{
	const std::string FOV_DEGREE_VAR       = "fov-degree";
	const std::string SENSOR_WIDTH_MM_VAR  = "sensor-width-mm";
	const std::string SENSOR_OFFSET_MM_VAR = "sensor-offset-mm";

	InputPrototype fovBasedInput;
	fovBasedInput.addReal(FOV_DEGREE_VAR);

	InputPrototype dimensionalInput;
	dimensionalInput.addReal(SENSOR_WIDTH_MM_VAR);
	dimensionalInput.addReal(SENSOR_OFFSET_MM_VAR);

	if(packet.isPrototypeMatched(fovBasedInput))
	{
		// Respect sensor dimensions; modify sensor offset to satisfy FoV requirement
		const auto fovDegree = packet.getReal(FOV_DEGREE_VAR);
		const auto halfFov   = math::to_radians(fovDegree) * 0.5_r;
		m_sensorWidthMM  = packet.getReal(SENSOR_WIDTH_MM_VAR, m_sensorWidthMM);
		m_sensorOffsetMM = (m_sensorWidthMM * 0.5_r) / std::tan(halfFov);
	}
	else if(packet.isPrototypeMatched(dimensionalInput))
	{
		m_sensorWidthMM  = packet.getReal(SENSOR_WIDTH_MM_VAR);
		m_sensorOffsetMM = packet.getReal(SENSOR_OFFSET_MM_VAR);
	}
	else
	{
		std::cerr << "warning: in PerspectiveReceiver::PerspectiveReceiver(), bad input format" << std::endl;
	}

	updateTransforms();
}

SdlTypeInfo PerspectiveReceiver::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RECEIVER, "perspective");
}

void PerspectiveReceiver::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
