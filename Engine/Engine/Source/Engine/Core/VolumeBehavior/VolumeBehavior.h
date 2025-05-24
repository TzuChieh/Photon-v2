#pragma once

#include "Engine/Core/VolumeBehavior/VolumeOptics.h"

namespace ph
{

class VolumeBehavior final
{
public:
	const VolumeOptics* getOptics() const;
	void setOptics(const VolumeOptics* optics);

private:
	const VolumeOptics* m_optics;
};

inline const VolumeOptics* VolumeBehavior::getOptics() const
{
	return m_optics;
}

}// end namespace ph
