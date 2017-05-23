#include "Core/Camera/PerspectiveCamera.h"
#include "Core/Ray.h"
#include "Core/Sample.h"
#include "Core/Camera/Film.h"
#include "FileIO/InputPacket.h"
#include "Math/Math.h"
#include "FileIO/InputPrototype.h"

#include <iostream>

namespace ph
{

PerspectiveCamera::~PerspectiveCamera() = default;

void PerspectiveCamera::onFilmSet(Film* newFilm)
{
	updateTransforms();
}

void PerspectiveCamera::updateTransforms()
{
	const hiReal rasterWidthPx   = static_cast<hiReal>(getFilm()->getWidthPx());
	const hiReal rasterHeightPx  = static_cast<hiReal>(getFilm()->getHeightPx());
	const hiReal filmAspectRatio = rasterWidthPx / rasterHeightPx;
	const hiReal filmWidthMM     = m_filmWidthMM;
	const hiReal filmHeightMM    = filmWidthMM / filmAspectRatio;
	const hiReal filmOffsetMM    = m_filmOffsetMM;

	TDecomposedTransform<hiReal> rasterToCameraTransform;
	rasterToCameraTransform.scale(-filmWidthMM / rasterWidthPx, -filmHeightMM / rasterHeightPx, 1);
	rasterToCameraTransform.translate(filmWidthMM / 2, filmHeightMM / 2, filmOffsetMM);

	std::vector<TDecomposedTransform<hiReal>> rootToLocal{m_cameraToWorldTransform, rasterToCameraTransform};
	m_rasterToWorld  = std::make_shared<StaticTransform>(StaticTransform::makeParentedForward(rootToLocal));
	m_rasterToCamera = std::make_shared<StaticTransform>(StaticTransform::makeForward(rasterToCameraTransform));
	m_cameraToWorld  = std::make_shared<StaticTransform>(StaticTransform::makeForward(m_cameraToWorldTransform));
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

		const real fovDegree = packet.getReal(NAME_FOV_DEGREE);
		const real halfFov   = Math::toRadians(fovDegree) * 0.5_r;
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
}

SdlTypeInfo PerspectiveCamera::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_CAMERA, "perspective");
}

ExitStatus PerspectiveCamera::ciExecute(const std::shared_ptr<PerspectiveCamera>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph