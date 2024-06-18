#include "Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Emitter/Query/DirectEnergyPdfQuery.h"

namespace ph
{

void PrimitivePosPdfInput::set(const PrimitivePosSampleInput& posInput, const SurfaceHit& X)
{
	// Use observation position provided by input (more specific), otherwise try to infer it
	const auto observationPos = posInput.getObservationPos()
		? posInput.getObservationPos()
		: inferObservationPos(X);

	set(
		X,
		observationPos,
		posInput.getUvwPdf());
}

void PrimitivePosPdfInput::set(
	const DirectEnergyPdfInput& pdfInput,
	const lta::PDF& uvwPdf)
{
	set(
		pdfInput.getXe(),
		pdfInput.getTargetPos(),
		uvwPdf);
}

std::optional<math::Vector3R> PrimitivePosPdfInput::inferObservationPos(const SurfaceHit& X)
{
	const auto& hitReason = X.getReason();
	if(hitReason.has(ESurfaceHitReason::IncidentRay))
	{
		return X.getIncidentRay().getOrigin();
	}
	else
	{
		return std::nullopt;
	}
}

}// end namespace ph
