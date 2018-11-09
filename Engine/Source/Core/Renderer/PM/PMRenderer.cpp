#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "FileIO/SDL/SdlResourcePack.h"
#include "Core/Renderer/PM/RayTracingWork.h"
#include "Core/Renderer/PM/TPhotonMappingWork.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/TRadianceEvaluationWork.h"
#include "Core/Renderer/PM/PMPhoton.h"
#include "FileIO/SDL/InputPacket.h"
#include "Utility/FixedSizeThreadPool.h"
#include "Utility/concurrent.h"

#include <numeric>

namespace ph
{

void PMRenderer::doUpdate(const SdlResourcePack& data)
{
	m_film = std::make_unique<HdrRgbFilm>(getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

	m_scene = &(data.visualWorld.getScene());
	m_camera = data.getCamera().get();
	m_sg = data.getSampleGenerator().get();

	m_pmStatistics.zero();
	
	/*Scene* scene,
		Camera* camera,
		std::unique_ptr<SampleGenerator> sampleGenerator,
		Viewpoint* viewpointBuffer,
		std::size_t numViewpoints*/

	/*std::size_t numPixels = getRenderWindowPx().calcArea() * m_perPixelSamples;
	std::vector<Viewpoint> viewpointBuffer(numPixels);

	std::cerr << "numPixels = " << numPixels << std::endl;
	std::cerr << "ray tracing work start" << std::endl;

	RayTracingWork rayTracingWork(m_scene, m_camera, m_sg->genCopied(1), viewpointBuffer.data(), numPixels, m_kernelRadius);
	rayTracingWork.doWork();

	std::cerr << "ray tracing work finished" << std::endl;
	std::cerr << "size of viewpoint buffer: " << sizeof(Viewpoint) * numPixels / 1024 / 1024 << " MB" << std::endl;

	std::size_t numPhotons = m_numPhotons;
	std::vector<Photon> photonBuffer(numPhotons);

	std::cerr << "numPhotons = " << numPhotons << std::endl;
	std::cerr << "photon mapping work start" << std::endl;

	std::size_t numEmittedPhotons;
	PhotonMappingWork photonMappingWork(m_scene, m_camera, m_sg->genCopied(1), photonBuffer.data(), numPhotons, &numEmittedPhotons);
	photonMappingWork.doWork();

	std::cerr << "photon mapping work finished" << std::endl;
	std::cerr << "size of photon buffer: " << sizeof(Photon) * numPhotons / 1024 / 1024 << " MB" << std::endl;

	std::cerr << "building photon map" << std::endl;

	PhotonMap photonMap(2, PhotonCenterCalculator());
	photonMap.build(std::move(photonBuffer));

	std::cerr << "photon map built" << std::endl;

	std::cerr << "estimating radiance" << std::endl;

	RadianceEstimateWork radianceEstimator(&photonMap, viewpointBuffer.data(), viewpointBuffer.size(), m_film.get(), numEmittedPhotons);
	radianceEstimator.doWork();

	std::cerr << "estimation complete" << std::endl;*/

	// DEBUG
	/*for(std::size_t y = 0; y < getRenderHeightPx(); ++y)
	{
		for(std::size_t x = 0; x < getRenderWidthPx(); ++x)
		{
			Viewpoint viewpoint = viewpointBuffer[y * getRenderWidthPx() + x];
			real filmXPx = viewpoint.filmNdcPos.x * static_cast<real>(getRenderWidthPx());
			real filmYPx = viewpoint.filmNdcPos.y * static_cast<real>(getRenderHeightPx());
			m_film->addSample(filmXPx, filmYPx, viewpointBuffer[y * getRenderWidthPx() + x].hit.getShadingNormal());
		}
	}*/
}

void PMRenderer::doRender()
{
	renderWithVanillaPM();
}

void PMRenderer::renderWithVanillaPM()
{
	std::size_t numPixels = getRenderWindowPx().calcArea() * m_perPixelSamples;
	std::vector<Viewpoint> viewpointBuffer(numPixels);


	std::cerr << "ray tracing work finished" << std::endl;
	std::cerr << "size of viewpoint buffer: " << sizeof(Viewpoint) * numPixels / 1024 / 1024 << " MB" << std::endl;

	std::size_t numPhotons = m_numPhotons;
	std::vector<PMPhoton> photonBuffer(numPhotons);

	std::cerr << "numPhotons = " << numPhotons << std::endl;
	std::cerr << "photon mapping work start" << std::endl;

	std::vector<std::size_t> numEmittedPhotons(getNumWorkers(), 0);
	std::size_t bufferOffset = 0;

	parallel_work(numPhotons, getNumWorkers(), 
		[this, &photonBuffer, &numEmittedPhotons](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			auto sampleGenerator = m_sg->genCopied(1);

			TPhotonMappingWork<PMPhoton> photonMappingWork(
				m_scene,
				m_camera,
				sampleGenerator.get(),
				&(photonBuffer[workStart]),
				workEnd - workStart,
				&(numEmittedPhotons[workerIdx]));
			photonMappingWork.doWork();
		});

	std::size_t totalPhotons = std::accumulate(numEmittedPhotons.begin(), numEmittedPhotons.end(), std::size_t(0));
	

	std::cerr << "photon mapping work finished" << std::endl;
	std::cerr << "size of photon buffer: " << sizeof(PMPhoton) * numPhotons / 1024 / 1024 << " MB" << std::endl;

	std::cerr << "building photon map" << std::endl;

	TPhotonMap<PMPhoton> photonMap(2, TPhotonCenterCalculator<PMPhoton>());
	photonMap.build(std::move(photonBuffer));

	std::cerr << "photon map built" << std::endl;

	std::cerr << "estimating radiance" << std::endl;

	std::vector<std::unique_ptr<HdrRgbFilm>> workerFilms(getNumWorkers());
	for(auto&& workerFilm : workerFilms)
	{
		workerFilm = std::make_unique<HdrRgbFilm>(
			getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);
	}

	parallel_work(getNumWorkers(), getNumWorkers(), 
		[this, &photonMap, &viewpointBuffer, &workerFilms, totalPhotons](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			TRadianceEvaluationWork<PMPhoton> radianceEstimator(
				&photonMap, 
				viewpointBuffer.data(), 
				viewpointBuffer.size(), 
				m_film.get(), 
				totalPhotons);

			auto workerSg = m_sg->genCopied(9999999);
			const Samples2DStage filmStage = workerSg->declare2DStage(getRenderWindowPx().calcArea());// FIXME: consider sample filter extent & size hints
			const bool isSamplePrepared = workerSg->prepareSampleBatch();
			PH_ASSERT(isSamplePrepared);

			auto& workerFilm = workerFilms[workerIdx];

			while(workerSg->prepareSampleBatch())
			{
				const Samples2D samples = workerSg->getSamples2D(filmStage);
				for(std::size_t i = 0; i < samples.numSamples(); ++i)
				{
					const Vector2R filmNdcPos = samples[i];

					Ray tracingRay;
					m_camera->genSensedRay(filmNdcPos, &tracingRay);
					tracingRay.reverse();

					HitProbe probe;
					if(m_scene->isIntersecting(tracingRay, &probe))
					{
						SurfaceHit surfaceHit(tracingRay, probe);
						const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
						const SurfaceOptics* surfaceOptics = metadata->getSurface().getOptics();

						// TODO: handle specular path

						const Vector3R L = tracingRay.getDirection().mul(-1);
						const SpectralStrength radiance = radianceEstimator.evaluateRadiance(surfaceHit, L, m_kernelRadius);

						
						const real filmXPx = filmNdcPos.x * static_cast<real>(workerFilm->getActualResPx().x);
						const real filmYPx = filmNdcPos.y * static_cast<real>(workerFilm->getActualResPx().y);
						workerFilm->addSample(filmXPx, filmYPx, radiance);
					}
				}

				std::lock_guard<std::mutex> m_filmLock(m_filmMutex);
				m_film->mergeWith(*workerFilm);
			}
		});

	std::cerr << "estimation complete" << std::endl;
}

ERegionStatus PMRenderer::asyncPollUpdatedRegion(Region* out_region)
{
	*out_region = getRenderWindowPx();
	return ERegionStatus::UPDATING;
}

RenderState PMRenderer::asyncQueryRenderState()
{
	return RenderState();
}

RenderProgress PMRenderer::asyncQueryRenderProgress()
{
	return RenderProgress(0, 0, 0);
}

void PMRenderer::asyncDevelopRegion(HdrRgbFrame& out_frame, const Region& region, EAttribute attribute)
{
	std::lock_guard<std::mutex> m_filmLock(m_filmMutex);
	m_film->develop(out_frame, region);
}

void PMRenderer::develop(HdrRgbFrame& out_frame, EAttribute attribute)
{
	m_film->develop(out_frame);
}

AttributeTags PMRenderer::supportedAttributes() const
{
	AttributeTags supported;
	supported.tag(EAttribute::LIGHT_ENERGY);
	return supported;
}

std::string PMRenderer::renderStateName(const RenderState::EType type, const std::size_t index) const
{
	PH_ASSERT_LT(index, RenderState::numStates(type));

	if(type == RenderState::EType::INTEGER)
	{
		return "";
	}
	else if(type == RenderState::EType::REAL)
	{
		switch(index)
		{
		//case 0: return "samples/second";
		default: return "";
		}
	}
	else
	{
		return "";
	}
}

// command interface

PMRenderer::PMRenderer(const InputPacket& packet) : 
	Renderer(packet),
	m_film(),
	m_scene(nullptr),
	m_camera(nullptr),
	m_sg(nullptr),
	m_filter(SampleFilterFactory::createBlackmanHarrisFilter()),

	m_mode(),
	m_numPhotons(),
	m_kernelRadius(),
	m_perPixelSamples(),

	m_filmMutex(),
	m_pmStatistics()
{
	const std::string& mode = packet.getString("mode", "vanilla");
	if(mode == "vanilla")
	{
		m_mode = EPMMode::VANILLA;
	}
	else if(mode == "progressive")
	{
		m_mode = EPMMode::PROGRESSIVE;
	}
	else if(mode == "stochastic-progressive")
	{
		m_mode = EPMMode::STOCHASTIC_PROGRESSIVE;
	}

	m_numPhotons = packet.getInteger("num-photons", 100000);
	m_kernelRadius = packet.getReal("radius", 0.1_r);
	m_perPixelSamples = packet.getInteger("per-pixel-samples", 1);
}

SdlTypeInfo PMRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "pm");
}

void PMRenderer::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<PMRenderer>(packet);
	}));
}

}// end namespace ph