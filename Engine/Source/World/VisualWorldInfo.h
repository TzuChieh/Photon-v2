#pragma once

#include "Core/Bound/AABB3D.h"

namespace ph
{

/*
	This class gathers information about visual world such as world bounds.
*/
class VisualWorldInfo final
{
public:
	AABB3D getRootActorsBound() const;

	void setRootActorsBound(const AABB3D& bound);

private:
	AABB3D m_rootActorsBound;
};

}// end namespace ph