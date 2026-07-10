#include "Engine/Actor/Material/Component/SurfaceLayerInfo.h"

#include <utility>

namespace ph
{

void SurfaceLayerInfo::setRoughnessMap(std::shared_ptr<Image> roughnessMap)
{
	m_roughnessMap = std::move(roughnessMap);
}

void SurfaceLayerInfo::setIorNMap(std::shared_ptr<Image> iorNMap)
{
	m_iorNMap = std::move(iorNMap);
}

void SurfaceLayerInfo::setIorKMap(std::shared_ptr<Image> iorKMap)
{
	m_iorKMap = std::move(iorKMap);
}

void SurfaceLayerInfo::setDepthMap(std::shared_ptr<Image> depthMap)
{
	m_depthMap = std::move(depthMap);
}

void SurfaceLayerInfo::setGMap(std::shared_ptr<Image> gMap)
{
	m_gMap = std::move(gMap);
}

void SurfaceLayerInfo::setSigmaAMap(std::shared_ptr<Image> sigmaAMap)
{
	m_sigmaAMap = std::move(sigmaAMap);
}

void SurfaceLayerInfo::setSigmaSMap(std::shared_ptr<Image> sigmaSMap)
{
	m_sigmaSMap = std::move(sigmaSMap);
}

}// end namespace ph
