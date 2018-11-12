#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PM/TViewpoint.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Core/Renderer/PM/TPhotonMap.h"

#include <type_traits>

namespace ph
{

template<typename Photon>
class TRadianceEvaluationWork : public RenderWork
{
	static_assert(std::is_base_of_v<TPhoton<Photon>, Photon>);

public:
	TRadianceEvaluationWork(
		const TPhotonMap<Photon>* photonMap,
		std::size_t               numPhotonPaths);

	SpectralStrength evaluateRadiance(
		const SurfaceHit& location,
		const Vector3R&   excitant,
		real              kernelRadius);

protected:
	const TPhotonMap<Photon>* getPhotonMap() const;
	const std::size_t         numPhotonPaths() const;

private:
	void doWork() override = 0;

	const TPhotonMap<Photon>* m_photonMap;
	//Viewpoint* m_viewpoints;
	std::size_t m_numViewpoints;
	HdrRgbFilm* m_film;
	std::size_t m_numPhotonPaths;
	std::vector<Photon> m_photonCache;
};

}// end namespace ph

#include "Core/Renderer/PM/TRadianceEvaluationWork.ipp"