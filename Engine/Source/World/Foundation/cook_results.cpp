#include "World/Foundation/cook_results.h"
#include "World/Foundation/PreCookResourceCollectionProxy.h"
#include "World/Foundation/ResourceCookReport.h"

namespace ph
{

PreCookResults::PreCookResults(
	ResourceCookReport&             cookReport,
	PreCookResourceCollectionProxy& resourceCollection)
	
	: m_cookReport        (cookReport)
	, m_resourceCollection(resourceCollection)
{}

}// end namespace ph
