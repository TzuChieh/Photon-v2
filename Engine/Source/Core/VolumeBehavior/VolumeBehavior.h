#pragma once

#include "Core/VolumeBehavior/VolumeOptics.h"
#include "Common/assertion.h"

#include <memory>

namespace ph
{

class VolumeBehavior final
{
public:
	const VolumeOptics* getOptics() const;
	void setOptics(const std::shared_ptr<VolumeOptics>& optics);

private:
	std::shared_ptr<VolumeOptics> m_optics;
};

inline 
const VolumeOptics* VolumeBehavior::getOptics() const
{
	return m_optics.get();
}

inline 
void VolumeBehavior::setOptics(const std::shared_ptr<VolumeOptics>& optics)
{
	PH_ASSERT(optics);

	m_optics = optics;
}

}// end namespace ph