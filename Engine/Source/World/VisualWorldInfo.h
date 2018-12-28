#pragma once

#include "Core/Bound/TAABB3D.h"

namespace ph
{

/*
	This class gathers information about visual world such as world bounds.
*/
class VisualWorldInfo final
{
	// TODO: we may add another bound that records current actor bounds on
	// each child actor cooking iteration

public:
	AABB3D getRootActorsBound() const;

	void setRootActorsBound(const AABB3D& bound);

private:
	AABB3D m_rootActorsBound;
};

}// end namespace ph