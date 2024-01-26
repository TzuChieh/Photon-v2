#include "SDL/SdlDependencyResolver.h"
#include "SDL/ISdlResource.h"
#include "SDL/Introspect/SdlClass.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlDependencyResolver, SDL);

SdlDependencyResolver::SdlDependencyResolver() :
	m_resourceInfos      (),
	m_queuedResources    (),
	m_resourceToInfoIndex()
{}

void SdlDependencyResolver::analyze(
	TSpanView<const ISdlResource*> resources,
	TSpanView<std::string> resourceNames)
{
	if(!resourceNames.empty())
	{
		if(resourceNames.size() != resources.size())
		{
			PH_LOG_WARNING(SdlDependencyResolver,
				"incomplete resource name info detected: {} resource names provided (expecting {})", 
				resourceNames.size(),
				resources.size());

			// Proceed as if no resource names were provided
			resourceNames = {};
		}
	}

	// Gather all resources for analyzing dependencies
	{
		m_resourceInfos.resize(resources.size());

		m_resourceToInfoIndex.clear();
		m_resourceToInfoIndex.reserve(resources.size());

		for(std::size_t i = 0; i < resources.size(); ++i)
		{
			ResourceInfo& resInfo = m_resourceInfos[i];
			resInfo.resource = resources[i];
			resInfo.name = resourceNames.empty() ? "" : resourceNames[i];

			m_resourceToInfoIndex[resources[i]] = i;
		}
	}

	PH_LOG_DEBUG(SdlDependencyResolver,
		"gathered {} resources to analyze for dependencies", m_resourceInfos.size());

	calcDispatchOrderFromTopologicalSort();
}

const ISdlResource* SdlDependencyResolver::next()
{
	if(m_queuedResources.empty())
	{
		return nullptr;
	}

	const ISdlResource* const resource = m_queuedResources.front();
	m_queuedResources.pop();
	return resource;
}

std::string_view SdlDependencyResolver::getResourceName(const ISdlResource* const resource) const
{
	const auto optIdx = getResourceInfoIdx(resource);

	// Return empty string if not available.
	//
	//       !!! IMPORTANT NOTE !!!
	// Do NOT use ternary conditional to return `std::string_view` if the resulting types are
	// different--if they both got promoted to `std::string`, a dangling view will be created!
	//
	if(optIdx)
	{
		return m_resourceInfos[*optIdx].name;
	}
	else
	{
		return "";
	}
}

void SdlDependencyResolver::calcDispatchOrderFromTopologicalSort()
{
	const std::size_t numResources = m_resourceInfos.size();

	// Every resource has an adjacency list storing the indices to the depending resources.
	// E.g., if resource "i" is the "j"-th resource depending on resource "k" (as "i" depends
	// on "k"), the relation will be represented as "adjacencyLists[k][j] = i"
	std::vector<std::vector<std::size_t>> adjacencyLists(numResources);
	std::vector<int> dependentCounts(numResources, 0);

	PH_LOG(SdlDependencyResolver, "building DAG for {} resources", numResources);

	// Builds the DAG (fill in adjacency lists)
	{
		std::vector<const ISdlResource*> tmpReferencedResources;
		std::size_t maxRefCount = 0;

		for(std::size_t resIdx = 0; resIdx < m_resourceInfos.size(); ++resIdx)
		{
			const ResourceInfo& resInfo = m_resourceInfos[resIdx];
			PH_ASSERT(resInfo.resource);

			const SdlClass* const sdlClass = resInfo.resource->getDynamicSdlClass();
			PH_ASSERT(sdlClass);

			tmpReferencedResources.clear();
			sdlClass->referencedResources(resInfo.resource, tmpReferencedResources);

			maxRefCount = std::max(tmpReferencedResources.size(), maxRefCount);

			for(std::size_t refIdx = 0; refIdx < tmpReferencedResources.size(); ++refIdx)
			{
				const ISdlResource* const referencedRes = tmpReferencedResources[refIdx];
				PH_ASSERT(referencedRes);

				if(referencedRes == resInfo.resource)
				{
					PH_LOG_WARNING(SdlDependencyResolver,
						"resource {} referenced itself, ignoring the reference", resInfo.name);
					continue;
				}

				const auto optReferencedResIdx = getResourceInfoIdx(referencedRes);
				if(!optReferencedResIdx)
				{
					PH_LOG_WARNING(SdlDependencyResolver,
						"resource {} referenced a resource that is not tracked by the analyzer, ignoring the reference "
						"(this typically happens if the referenced resource is not submitted for analysis)",
						resInfo.name);
					continue;
				}

				// Referenced resource index and current resource index must be different
				// (no self referencing)
				PH_ASSERT_NE(resIdx, *optReferencedResIdx);

				// Record resource <resIdx> depends on resource <*optReferencedResIdx>
				adjacencyLists[*optReferencedResIdx].push_back(resIdx);
				dependentCounts[resIdx]++;
			}
		}

		PH_LOG_DEBUG(SdlDependencyResolver,
			"DAG building done, max references/degree = {}", maxRefCount);
	}// end DAG building

	// Start topological sorting by finding resources without any dependency
	std::queue<std::size_t> independentResIndices;
	for(std::size_t resIdx = 0; resIdx < dependentCounts.size(); ++resIdx)
	{
		if(dependentCounts[resIdx] == 0)
		{
			independentResIndices.push(resIdx);
		}
	}

	PH_LOG_DEBUG(SdlDependencyResolver,
		"{} resources are already independent", independentResIndices.size());

	// Main topological sorting that produces a valid resource dispatch order
	m_queuedResources = std::queue<const ISdlResource*>();
	while(!independentResIndices.empty())
	{
		const std::size_t independentResIdx = independentResIndices.front();
		independentResIndices.pop();

		m_queuedResources.push(m_resourceInfos[independentResIdx].resource);

		// Mark the resource as dispatched
		PH_ASSERT_EQ(dependentCounts[independentResIdx], 0);
		dependentCounts[independentResIdx] = -1;

		// Update dependent counts of resources depending on the dispatched resource
		// (remove dependency "edge")
		const std::vector<std::size_t>& dependingResIndices = adjacencyLists[independentResIdx];
		for(std::size_t idx = 0; idx < dependingResIndices.size(); ++idx)
		{
			const std::size_t dependingResIdx = dependingResIndices[idx];

			PH_ASSERT_GT(dependentCounts[dependingResIdx], 0);
			dependentCounts[dependingResIdx]--;

			// If the depending resource is now independent, queue it for dispatchment
			if(dependentCounts[dependingResIdx] == 0)
			{
				independentResIndices.push(dependingResIdx);
			}
		}
	}

	// Now a dispatch order that satisfies the dependencies between all resources
	// is established. We check each resource's dependent count to see if there is
	// any error.

	if(m_queuedResources.size() != numResources)
	{
		PH_LOG_WARNING(SdlDependencyResolver, "queued {} resources while there are {} in total",
			m_queuedResources.size(), numResources);
	}

	for(std::size_t resIdx = 0; resIdx < dependentCounts.size(); ++resIdx)
	{
		const auto dependentCount = dependentCounts[resIdx];
		PH_ASSERT_NE(dependentCount, 0);

		if(dependentCount != -1)
		{
			const ResourceInfo& resInfo = m_resourceInfos[resIdx];
			PH_LOG_WARNING(SdlDependencyResolver,
				"possible cyclic references detected: resource {} has {} references that cannot be resolved",
				resInfo.name, dependentCount);
		}
	}
}

std::optional<std::size_t> SdlDependencyResolver::getResourceInfoIdx(const ISdlResource* const resource) const
{
	const auto result = m_resourceToInfoIndex.find(resource);
	if(result == m_resourceToInfoIndex.end())
	{
		return std::nullopt;
	}

	const std::size_t infoIdx = result->second;
	PH_ASSERT_LT(infoIdx, m_resourceInfos.size());
	return infoIdx;
}

}// end namespace ph
