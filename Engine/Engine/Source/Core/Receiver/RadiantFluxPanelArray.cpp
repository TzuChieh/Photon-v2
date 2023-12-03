//#include "Core/Receiver/RadiantFluxPanelArray.h"
//#include "Common/assertion.h"
//#include "Math/TVector2.h"
//#include "Math/TVector3.h"
//#include "Math/Geometry/TAABB2D.h"
//#include "Core/Ray.h"
//#include "Math/Transform/StaticRigidTransform.h"
//#include "Math/Geometry/THemisphere.h"
//#include "Math/Random.h"
//#include "Common/Logger.h"
//#include "Math/math.h"
//
//#include <limits>
//
//namespace ph
//{
//
//namespace
//{
//	Logger logger(LogSender("Radiant Flux Panel Array"));
//}
//
//Spectrum RadiantFluxPanelArray::receiveRay(const math::Vector2D& rasterCoord, Ray* const out_ray) const
//{
//	PH_ASSERT(out_ray);
//	PH_ASSERT(m_localToWorld);
//	PH_ASSERT_GT(getRasterResolution().product(), 0);
//
//	const auto    rasterRes     = getRasterResolution();
//	const auto    iRasterCoord  = math::Vector2S(rasterCoord);
//	//const float64 rasterPickPdf = 1.0 / rasterRes.product();
//	const float64 rasterPickPdf = 1.0;
//
//	// Out-of-bound raster coordinates imply invalid panel surface position
//	if(iRasterCoord.x < 0 || iRasterCoord.x >= rasterRes.x ||
//	   iRasterCoord.y < 0 || iRasterCoord.y >= rasterRes.y)
//	{
//		return Spectrum(0);
//	}
//	
//	const auto mapIndex = iRasterCoord.y * rasterRes.x + iRasterCoord.x;
//	PH_ASSERT_LT(mapIndex, m_rasterCoordToPanelIndex.size());
//	const auto panelIndex = m_rasterCoordToPanelIndex[mapIndex];
//
//	const RadiantFluxPanel& panel = m_panels[panelIndex];
//
//	const auto panelRasterCoord = math::Vector2D(
//		math::fractional_part(rasterCoord.x),
//		math::fractional_part(rasterCoord.y));
//
//	Ray localRay;
//	const Spectrum localWeight = panel.receiveRay(panelRasterCoord, &localRay);
//
//	m_localToWorld->transform(localRay, out_ray);
//
//	return localWeight / static_cast<real>(rasterPickPdf);
//}
//
//void RadiantFluxPanelArray::evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const
//{
//	PH_ASSERT_UNREACHABLE_SECTION();
//}
//
//}// end namespace ph
