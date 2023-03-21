#include "DataIO/SDL/SdlReferenceResolver.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/SceneDescription.h"
#include "DataIO/SDL/Introspect/SdlClass.h"

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlReferenceResolver, SDL);

SdlReferenceResolver::SdlReferenceResolver() :
	m_resourceInfos      (),
	m_queuedResources    (),
	m_resourceToInfoIndex()
{}

void SdlReferenceResolver::analyze(const SceneDescription& scene)
{
	// Gather all resources in the scene for analyzing dependencies
	{
		std::vector<const ISdlResource*> resources;
		std::vector<std::string_view> names;
		scene.getResources().listAll(&resources, &names);

		PH_ASSERT_EQ(resources.size(), names.size());

		m_resourceInfos.resize(resources.size());

		m_resourceToInfoIndex.clear();
		m_resourceToInfoIndex.reserve(resources.size());

		for(std::size_t i = 0; i < resources.size(); ++i)
		{
			ResourceInfo& resInfo = m_resourceInfos[i];
			resInfo.resource = resources[i];
			resInfo.name = names[i];

			m_resourceToInfoIndex[resources[i]] = i;
		}
	}

	PH_LOG(SdlReferenceResolver, 
		"gathered {} resources to analyze for dependencies", m_resourceInfos.size());

	calcDispatchOrderFromTopologicalSort();
}

const ISdlResource* SdlReferenceResolver::dispatch()
{
	if(m_queuedResources.empty())
	{
		return nullptr;
	}

	const ISdlResource* const resource = m_queuedResources.front();
	m_queuedResources.pop();
	return resource;
}

std::string_view SdlReferenceResolver::getResourceName(const ISdlResource* const resource) const
{
	const auto optIdx = getResourceInfoIdx(resource);
	return optIdx ? m_resourceInfos[*optIdx].name : "";
}

void SdlReferenceResolver::calcDispatchOrderFromTopologicalSort()
{
	const std::size_t numResources = m_resourceInfos.size();

	// Every resource has an adjacency list storing the indices to the depending resources.
	// E.g., if resource "i" is the "j"-th resource depending on resource "k" (as "i" depends
	// on "k"), the relation will be represented as "adjacencyLists[k][j] = i"
	std::vector<std::vector<std::size_t>> adjacencyLists(numResources);
	std::vector<int> dependentCounts(numResources, 0);

	PH_LOG(SdlReferenceResolver, "building DAG for {} resources", numResources);

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
					PH_LOG_WARNING(SdlReferenceResolver,
						"resource {} referenced itself, ignoring the reference", resInfo.name);
					continue;
				}

				const auto optReferencedResIdx = getResourceInfoIdx(referencedRes);
				if(!optReferencedResIdx)
				{
					PH_LOG_WARNING(SdlReferenceResolver,
						"resource {} referenced a resource that is not tracked by the analyzed SceneDescription, ignoring the reference",
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

		PH_LOG(SdlReferenceResolver, "DAG building done, max references/degree = {}", maxRefCount);
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

	PH_LOG(SdlReferenceResolver, "{} resources are already independent", independentResIndices.size());

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
		PH_LOG_WARNING(SdlReferenceResolver, "queued {} resources while there are {} in total", 
			m_queuedResources.size(), numResources);
	}

	for(std::size_t resIdx = 0; resIdx < dependentCounts.size(); ++resIdx)
	{
		const auto dependentCount = dependentCounts[resIdx];
		PH_ASSERT_NE(dependentCount, 0);

		if(dependentCount != -1)
		{
			const ResourceInfo& resInfo = m_resourceInfos[resIdx];
			PH_LOG_WARNING(SdlReferenceResolver, 
				"possible cyclic references detected: resource {} has {} references that cannot be resolved",
				resInfo.name, dependentCount);
		}
	}
}

std::optional<std::size_t> SdlReferenceResolver::getResourceInfoIdx(const ISdlResource* const resource) const
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
