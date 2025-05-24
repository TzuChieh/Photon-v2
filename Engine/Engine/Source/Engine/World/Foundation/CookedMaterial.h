#pragma once

#include <vector>

namespace ph
{

class SurfaceOptics;
class VolumeOptics;

enum class EVolumeComposition
{
	General,
	Interior,
	Exterior,
};

struct VolumeComposition final
{
	const VolumeOptics* optics = nullptr;
	EVolumeComposition type = {};
};

class CookedMaterial final
{
public:
	const SurfaceOptics* surfaceOptics = nullptr;
	std::vector<VolumeComposition> volumeCompositions;

	/*!
	Find compatible optics from the volume compositions. If an optics matches both interior
	and exterior, it will be returned as interior.
	@param out_interior[out] Pointer to the first interior volume optics, or `nullptr` if none exist.
	@param out_exterior[out] Pointer to the first exterior volume optics, or `nullptr` if none exist.
	*/
	void findFirstCompatibleOptics(
		const VolumeOptics** out_interior = nullptr,
		const VolumeOptics** out_exterior = nullptr) const;
};

}// end namespace ph
