#pragma once

#include "Core/Receiver/Receiver.h"
#include "Core/Receiver/RadiantFluxPanel.h"
#include "Common/primitive_type.h"

#include <memory>
#include <vector>

namespace ph::math { class Transform; }

namespace ph
{

class RadiantFluxPanelArray : public Receiver, public TCommandInterface<RadiantFluxPanelArray>
{
public:
	// TODO: ordinary ctors

	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;
	void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	std::vector<RadiantFluxPanel>    m_panels;
	std::vector<math::Vector2S>      m_rasterIndices;
	std::shared_ptr<math::Transform> m_receiverToWorld;// FIXME: should be rigid

// Command Interface
public:
	explicit RadiantFluxPanelArray(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
