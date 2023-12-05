//#pragma once
//
//#include "Core/Receiver/Receiver.h"
//#include "Core/Receiver/RadiantFluxPanel.h"
//
//#include <memory>
//#include <vector>
//
//namespace ph::math { class RigidTransform; }
//
//namespace ph
//{
//
//class RadiantFluxPanelArray : public Receiver
//{
//public:
//	// TODO: ordinary ctors
//
//	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;
//	void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;
//
//private:
//	std::vector<RadiantFluxPanel>         m_panels;
//	std::vector<std::size_t>              m_rasterCoordToPanelIndex;
//	std::shared_ptr<math::RigidTransform> m_localToWorld;
//};
//
//}// end namespace ph
