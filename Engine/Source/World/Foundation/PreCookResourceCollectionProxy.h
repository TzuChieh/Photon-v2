#pragma once

#include "World/Foundation/CookedResourceCollection.h"

namespace ph
{

class PreCookResourceCollectionProxy final
{
public:
	explicit PreCookResourceCollectionProxy(CookedResourceCollection& resources);

	// TODO: subcollection accessors

private:
	CookedResourceCollection& m_resources;
};

// In-header Implementations:

inline PreCookResourceCollectionProxy::PreCookResourceCollectionProxy(CookedResourceCollection& resources) :
	m_resources(resources)
{}

}// end namespace ph
