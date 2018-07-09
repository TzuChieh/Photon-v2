#pragma once

#include "Core/Camera/Camera.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <iostream>

namespace ph
{

class Ray;
class Sample;
class SampleGenerator;
class Film;
class InputPacket;

class PerspectiveCamera : public Camera, public TCommandInterface<PerspectiveCamera>
{
public:
	virtual ~PerspectiveCamera() = 0;

	virtual void genSensedRay(const Vector2R& filmNdcPos, Ray* out_ray) const = 0;
	virtual void evalEmittedImportanceAndPdfW(
		const Vector3R& targetPos, 
		Vector2R* const out_filmCoord,
		Vector3R* const out_importance, 
		real* out_filmArea, 
		real* const out_pdfW) const = 0;

	void setAspectRatio(real ratio) override;

protected:
	std::shared_ptr<Transform> m_cameraToWorld;
	std::shared_ptr<Transform> m_filmToCamera;
	std::shared_ptr<Transform> m_filmToWorld;
	
private:
	real m_filmWidthMM;
	real m_filmOffsetMM;

	void updateTransforms();

// command interface
public:
	explicit PerspectiveCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline void PerspectiveCamera::setAspectRatio(const real ratio)
{
	Camera::setAspectRatio(ratio);

	updateTransforms();
}

}// end namespace ph