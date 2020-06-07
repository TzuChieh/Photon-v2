#include "Core/Receiver/RadiantFluxPanelArray.h"
#include "Common/assertion.h"
#include "DataIO/SDL/InputPacket.h"
#include "DataIO/SDL/SdlLoader.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TAABB2D.h"
#include "Core/Ray.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/Random.h"
#include "Common/Logger.h"
#include "Math/math.h"

#include <limits>

namespace ph
{

namespace
{
	Logger logger(LogSender("Radiant Flux Panel Array"));
}

Spectrum RadiantFluxPanelArray::receiveRay(const math::Vector2D& rasterCoord, Ray* const out_ray) const
{
	PH_ASSERT(out_ray);
	PH_ASSERT(m_localToWorld);
	PH_ASSERT_GT(getRasterResolution().product(), 0);

	const auto    rasterRes     = getRasterResolution();
	const auto    iRasterCoord  = math::Vector2S(rasterCoord);
	const float64 rasterPickPdf = 1.0 / rasterRes.product();

	// Out-of-bound raster coordinates imply invalid panel surface position
	if(iRasterCoord.x < 0 || iRasterCoord.x >= rasterRes.x ||
	   iRasterCoord.y < 0 || iRasterCoord.y >= rasterRes.y)
	{
		return Spectrum(0);
	}
	
	const auto mapIndex = iRasterCoord.y * rasterRes.x + iRasterCoord.x;
	PH_ASSERT_LT(mapIndex, m_rasterCoordToPanelIndex.size());
	const auto panelIndex = m_rasterCoordToPanelIndex[mapIndex];

	const RadiantFluxPanel& panel = m_panels[panelIndex];

	const auto panelRasterCoord = math::Vector2D(
		math::fractional_part(rasterCoord.x),
		math::fractional_part(rasterCoord.y));

	Ray localRay;
	const Spectrum localWeight = panel.receiveRay(panelRasterCoord, &localRay);

	m_localToWorld->transform(localRay, out_ray);

	return localWeight / rasterPickPdf;
}

void RadiantFluxPanelArray::evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const
{
	PH_ASSERT_UNREACHABLE_SECTION();
}

// Command Interface

RadiantFluxPanelArray::RadiantFluxPanelArray(const InputPacket& packet) :

	Receiver(packet),

	m_panels(),
	m_rasterCoordToPanelIndex()
{
	const auto rasterRes = getRasterResolution();

	m_rasterCoordToPanelIndex.resize(rasterRes.product(), rasterRes.product());

	const auto panelWidths = packet.getRealArray("panel-widths", 
		std::vector<real>(), DataTreatment::REQUIRED());

	const auto panelHeights = packet.getRealArray("panel-heights",
		std::vector<real>(), DataTreatment::REQUIRED());

	const auto positions = packet.getVector3Array("positions",
		std::vector<math::Vector3R>(), DataTreatment::REQUIRED());

	const auto directions = packet.getVector3Array("directions",
		std::vector<math::Vector3R>(), DataTreatment::REQUIRED());

	const auto upAxes = packet.getVector3Array("up-axes",
		std::vector<math::Vector3R>(), DataTreatment::REQUIRED());

	// FIXME: int array
	const auto rasterIndicesX = packet.getRealArray("rasater-indices-x",
		std::vector<real>(), DataTreatment::REQUIRED());

	// FIXME: int array
	const auto rasterIndicesY = packet.getRealArray("rasater-indices-y",
		std::vector<real>(), DataTreatment::REQUIRED());

	if(panelWidths.size() != panelHeights.size()   ||
	   panelWidths.size() != positions.size()      ||
	   panelWidths.size() != directions.size()     ||
	   panelWidths.size() != upAxes.size()         ||
	   panelWidths.size() != rasterIndicesX.size() ||
	   panelWidths.size() != rasterIndicesY.size())
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"array size mismatch");
		return;
	}

	for(std::size_t i = 0; i < panelWidths.size(); ++i)
	{
		m_panels.push_back(RadiantFluxPanel(
			{panelWidths[i], panelHeights[i]},
			positions[i],
			directions[i],
			upAxes[i]));

		const auto rasterIndex = math::Vector2S(
			static_cast<std::size_t>(rasterIndicesX[i]),
			static_cast<std::size_t>(rasterIndicesY[i]));

		if(rasterIndex.x < 0 || rasterIndex.x >= rasterRes.x ||
		   rasterIndex.y < 0 || rasterIndex.y >= rasterRes.y)
		{
			logger.log(ELogLevel::FATAL_ERROR,
				"raster index out of bound; bound = " + rasterRes.toString() + ", "
				"index = " + rasterIndex.toString());
			return;
		}

		m_rasterCoordToPanelIndex[rasterIndex.y * rasterRes.x + rasterIndex.x] = i;
	}

	m_localToWorld = std::make_shared<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(m_receiverToWorldDecomposed));
}

SdlTypeInfo RadiantFluxPanelArray::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RECEIVER, "radiant-flux-panel-array");
}

void RadiantFluxPanelArray::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(
		SdlLoader([](const InputPacket& packet)
		{
			return std::make_unique<RadiantFluxPanelArray>(packet);
		}));
}

}// end namespace ph
