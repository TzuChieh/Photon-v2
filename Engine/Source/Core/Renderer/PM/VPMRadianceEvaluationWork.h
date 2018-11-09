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
class PMRenderer;

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

	void setPMStatistics(PMStatistics* statistics);
	void setPMRenderer(PMRenderer* renderer);
	void setKernelRadius(real radius);

private:
	void doWork() override;

	const Scene*     m_scene;
	const Camera*    m_camera;
	SampleGenerator* m_sampleGenerator;
	HdrRgbFilm*      m_film;
	PMStatistics*    m_statistics;
	PMRenderer*      m_renderer;
	real             m_kernelRadius;
};

// In-header Implementations:

inline void VPMRadianceEvaluationWork::setPMStatistics(PMStatistics* const statistics)
{
	m_statistics = statistics;
}

inline void VPMRadianceEvaluationWork::setPMRenderer(PMRenderer* const renderer)
{
	m_renderer = renderer;
}

inline void VPMRadianceEvaluationWork::setKernelRadius(const real radius)
{
	PH_ASSERT_GT(radius, 0.0_r);

	m_kernelRadius = radius;
}

}// end namespace ph