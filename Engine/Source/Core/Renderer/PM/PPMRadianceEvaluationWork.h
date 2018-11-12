#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Core/Renderer/PM/TRadianceEvaluationWork.h"
#include "Core/Renderer/PM/PPMPhoton.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/PM/PPMViewpoint.h"

#include <vector>

namespace ph
{

class Scene;
class PMStatistics;
class PMRenderer;

class PPMRadianceEvaluationWork : public TRadianceEvaluationWork<PPMPhoton>
{
public:
	PPMRadianceEvaluationWork(
		const TPhotonMap<PPMPhoton>* photonMap,
		std::size_t                  numPhotonPaths,
		HdrRgbFilm*                  film,
		PPMViewpoint* viewpoints,
		std::size_t numViewpoints,
		const Scene* scene);

	void setPMStatistics(PMStatistics* statistics);

private:
	void doWork() override;

	HdrRgbFilm*      m_film;
	PMStatistics*    m_statistics;
	PPMViewpoint* m_viewpoints;
	std::size_t m_numViewpoints;
	const Scene* m_scene;
};

// In-header Implementations:

inline void PPMRadianceEvaluationWork::setPMStatistics(PMStatistics* const statistics)
{
	m_statistics = statistics;
}

}// end namespace ph