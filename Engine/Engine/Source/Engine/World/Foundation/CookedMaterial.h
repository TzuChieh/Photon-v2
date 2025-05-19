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
	@return Pointer to the first interior volume optics, or `nullptr` if none exist.
	*/
	const VolumeOptics* getInteriorOptics() const;

	/*!
	@return Pointer to the first exterior volume optics, or `nullptr` if none exist.
	*/
	const VolumeOptics* getExteriorOptics() const;
};

inline const VolumeOptics* CookedMaterial::getInteriorOptics() const
{
	for(const VolumeComposition& comp : volumeCompositions)
	{
		if(!comp.isExterior && comp.optics)
		{
			return comp.optics;
		}
	}
	return nullptr;
}

inline const VolumeOptics* CookedMaterial::getExteriorOptics() const
{
	for(const VolumeComposition& comp : volumeCompositions)
	{
		if(comp.isExterior && comp.optics)
		{
			return comp.optics;
		}
	}
	return nullptr;
}

}// end namespace ph
