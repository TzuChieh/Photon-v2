#pragma once

#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Core/Renderer/PM/TViewpoint.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Math/Color/Spectrum.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <type_traits>

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
template<CPhoton Photon, CViewpoint Viewpoint>
class TPPMRadianceEvaluationWork : public RenderWork
{
public:
	/*!
	@param totalPhotonPaths Number of photon paths of all time (accumulated). This term is the
	@f$ N_e @f$ term in the original paper, not the same as `photonMap->numPaths`.
	*/
	TPPMRadianceEvaluationWork(
		TSpan<Viewpoint>               viewpoints,
		const TPhotonMap<Photon>*      photonMap,
		const Scene*                   scene,
		TSamplingFilm<math::Spectrum>* film,
		std::size_t                    totalPhotonPaths);

	void setStatistics(PMAtomicStatistics* statistics);
	void setAlpha(real alpha);

private:
	void doWork() override;

	TSpan<Viewpoint>               m_viewpoints;
	const Scene*                   m_scene;
	const TPhotonMap<Photon>*      m_photonMap;
	TSamplingFilm<math::Spectrum>* m_film;
	real                           m_rcpTotalPhotonPaths;
	PMAtomicStatistics*            m_statistics;
	real                           m_alpha;

	void sanitizeVariables();
};

// In-header Implementations:

template<CPhoton Photon, CViewpoint Viewpoint>
inline void TPPMRadianceEvaluationWork<Photon, Viewpoint>
::setStatistics(PMAtomicStatistics* const statistics)
{
	m_statistics = statistics;
}

template<CPhoton Photon, CViewpoint Viewpoint>
inline void TPPMRadianceEvaluationWork<Photon, Viewpoint>
::setAlpha(const real alpha)
{
	m_alpha = alpha;
}

}// end namespace ph

#include "Core/Renderer/PM/TPPMRadianceEvaluationWork.ipp"
