#pragma once

#include "Core/Renderer/PM/TRadianceEvaluationWork.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/PM/FullViewpoint.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <vector>

namespace ph
{

class Scene;
class PMAtomicStatistics;

/*!
Implements the progressive photon mapping technique developed by 
Hachisuka et al. The radiance evaluation process progressively shrinks 
the kernel radius and increases the number of photons, reaching a 
consistent result in the limit. 

References:
[1] Hachisuka et al., "Progressive Photon Mapping", ACM SIGGRAPH Asia 2008.
*/
class PPMRadianceEvaluationWork : public TRadianceEvaluationWork<FullPhoton>
{
	// TODO: templatize photon and viewpoint type

public:
	PPMRadianceEvaluationWork(
		const TPhotonMap<FullPhoton>* photonMap,
		std::size_t                   numPhotonPaths,
		HdrRgbFilm*                   film,
		FullViewpoint*                viewpoints,
		std::size_t                   numViewpoints,
		const Scene*                  scene);

	void setStatistics(PMAtomicStatistics* statistics);
	void setAlpha(real alpha);

private:
	void doWork() override;

	HdrRgbFilm*         m_film;
	PMAtomicStatistics* m_statistics;
	FullViewpoint*      m_viewpoints;
	std::size_t         m_numViewpoints;
	const Scene*        m_scene;
	real                m_alpha;

	void sanitizeVariables();
};

// In-header Implementations:

inline void PPMRadianceEvaluationWork::setStatistics(PMAtomicStatistics* const statistics)
{
	m_statistics = statistics;
}

inline void PPMRadianceEvaluationWork::setAlpha(const real alpha)
{
	m_alpha = alpha;
}

}// end namespace ph
