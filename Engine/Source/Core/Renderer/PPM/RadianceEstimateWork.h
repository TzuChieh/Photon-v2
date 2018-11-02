#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PPM/PhotonMap.h"
#include "Core/Renderer/PPM/Viewpoint.h"
#include "Core/Filmic/HdrRgbFilm.h"

namespace ph
{

class RadianceEstimateWork : public RenderWork
{
public:
	RadianceEstimateWork(
		const PhotonMap* photonMap,
		Viewpoint* viewpoints,
		std::size_t numViewpoints,
		HdrRgbFilm* film);

	void doWork() override;

private:
	const PhotonMap* m_photonMap;
	Viewpoint* m_viewpoints;
	std::size_t m_numViewpoints;
	HdrRgbFilm* m_film;
};

}// end namespace ph