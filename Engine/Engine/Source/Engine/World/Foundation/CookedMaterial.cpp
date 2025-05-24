#include "Engine/World/Foundation/CookedMaterial.h"

#include <Common/assertion.h>

namespace ph
{

void CookedMaterial::findFirstCompatibleOptics(
	const VolumeOptics** out_interior,
	const VolumeOptics** out_exterior) const
{
	PH_ASSERT(out_interior || out_exterior);

	if(out_interior)
	{
		*out_interior = nullptr;
	}

	if(out_exterior)
	{
		*out_exterior = nullptr;
	}

	for(const VolumeComposition& comp : volumeCompositions)
	{
		if(!comp.optics)
		{
			continue;
		}

		// General type can match both interior and exterior
		if(comp.type == EVolumeComposition::General)
		{
			// Interior has higher precedence
			if(out_interior && !*out_interior)
			{
				*out_interior = comp.optics;
			}
			else if(out_exterior && !*out_exterior)
			{
				*out_exterior = comp.optics;
			}
		}
		else if(comp.type == EVolumeComposition::Interior)
		{
			if(out_interior && !*out_interior)
			{
				*out_interior = comp.optics;
			}
		}
		else if(comp.type == EVolumeComposition::Exterior)
		{
			if(out_exterior && !*out_exterior)
			{
				*out_exterior = comp.optics;
			}
		}
	}
}

}// end namespace ph
