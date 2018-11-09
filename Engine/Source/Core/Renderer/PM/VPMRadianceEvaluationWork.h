#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Core/Renderer/PM/TRadianceEvaluationWork.h"
#include "Core/Renderer/PM/VPMPhoton.h"
#include "Core/Filmic/HdrRgbFilm.h"

#include <vector>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;
class PMStatistics;

class VPMRadianceEvaluationWork : public TRadianceEvaluationWork<VPMPhoton>
{
public:
	VPMRadianceEvaluationWork(
		const TPhotonMap<VPMPhoton>* photonMap,
		std::size_t                  numPhotonPaths,
		const Scene*                 scene,
		const Camera*                camera,
		SampleGenerator*             sampleGenerator,
		HdrRgbFilm*                  film);

	void doWork() override;

	void setPMStatistics(PMStatistics* statistics);
	void setKernelRadius(real radius);

private:
	const Scene*     m_scene;
	const Camera*    m_camera;
	SampleGenerator* m_sampleGenerator;
	HdrRgbFilm*      m_film;
	PMStatistics*    m_statistics;
	real             m_kernelRadius;
};

// In-header Implementations:

void VPMRadianceEvaluationWork::setPMStatistics(PMStatistics* const statistics)
{
	m_statistics = statistics;
}

void VPMRadianceEvaluationWork::setKernelRadius(const real radius)
{
	PH_ASSERT_GT(radius, 0.0_r);

	m_kernelRadius = radius;
}

}// end namespace ph