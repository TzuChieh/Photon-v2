#include "Core/Camera/PerspectiveCamera.h"
#include "Core/Ray.h"
#include "Core/Sample.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/math.h"
#include "FileIO/SDL/InputPrototype.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

void PerspectiveCamera::updateTransforms()
{
	const hiReal filmWidthMM  = m_filmWidthMM;
	const hiReal filmHeightMM = filmWidthMM / getAspectRatio();
	PH_ASSERT(filmWidthMM > 0.0_r && filmHeightMM >= 0.0_r);

	math::TDecomposedTransform<hiReal> filmToCameraTransform;
	filmToCameraTransform.scale(-filmWidthMM, -filmHeightMM, 1);
	filmToCameraTransform.translate(filmWidthMM / 2, filmHeightMM / 2, m_filmOffsetMM);

	std::vector<math::TDecomposedTransform<hiReal>> rootToLocal{m_cameraToWorldTransform, filmToCameraTransform};
	m_filmToWorld   = std::make_shared<math::StaticAffineTransform>(math::StaticAffineTransform::makeParentedForward(rootToLocal));
	m_filmToCamera  = std::make_shared<math::StaticAffineTransform>(math::StaticAffineTransform::makeForward(filmToCameraTransform));
	m_cameraToWorld = std::make_shared<math::StaticAffineTransform>(math::StaticAffineTransform::makeForward(m_cameraToWorldTransform));
}

// command interface

PerspectiveCamera::PerspectiveCamera(const InputPacket& packet) :
	Camera(packet), 
	m_filmWidthMM(36.0), m_filmOffsetMM(36.0)
{
	const std::string NAME_FOV_DEGREE     = "fov-degree";
	const std::string NAME_FILM_WIDTH_MM  = "film-width-mm";
	const std::string NAME_FILM_OFFSET_MM = "film-offset-mm";

	InputPrototype fovBasedInput;
	fovBasedInput.addReal(NAME_FOV_DEGREE);

	InputPrototype dimensionalInput;
	dimensionalInput.addReal(NAME_FILM_WIDTH_MM);
	dimensionalInput.addReal(NAME_FILM_OFFSET_MM);

	if(packet.isPrototypeMatched(fovBasedInput))
	{
		// Respect film dimensions; modify film offset to satisfy FOV requirement.
		const auto fovDegree = packet.getReal(NAME_FOV_DEGREE);
		const auto halfFov   = math::to_radians(fovDegree) * 0.5_r;
		m_filmWidthMM  = packet.getReal(NAME_FILM_WIDTH_MM, m_filmWidthMM);
		m_filmOffsetMM = (m_filmWidthMM * 0.5_r) / std::tan(halfFov);
	}
	else if(packet.isPrototypeMatched(dimensionalInput))
	{
		m_filmWidthMM  = packet.getReal(NAME_FILM_WIDTH_MM);
		m_filmOffsetMM = packet.getReal(NAME_FILM_OFFSET_MM);
	}
	else
	{
		std::cerr << "warning: in PerspectiveCamera::PerspectiveCamera(), bad input format" << std::endl;
	}

	updateTransforms();
}

SdlTypeInfo PerspectiveCamera::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_CAMERA, "perspective");
}

void PerspectiveCamera::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph
