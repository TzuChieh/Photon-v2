#pragma once

#include "Utility/INoCopyAndMove.h"
#include "Utility/TUniquePtrVector.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "DataIO/SDL/SdlResourceId.h"
#include "Common/logging.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <utility>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(CookedResourceCollectionBase, World);

/*! @brief Provides thread-safe cooked data creation and storage.
All methods are thread-safe to call, however manipulating the created data is not thread-safe unless
stated explicitly.
*/
class CookedResourceCollectionBase : private INoCopyAndMove
{
public:
	virtual ~CookedResourceCollectionBase() = default;

	virtual std::string getStats() const = 0;

protected:
	template<typename CookedType>
	using TSdlResourceIdMap = std::unordered_map<SdlResourceId, std::unique_ptr<CookedType>>;

	template<typename DerivedType, typename BaseType, typename... DeducedArgs>
	static DerivedType* makeCookedResource(
		TSynchronized<TUniquePtrVector<BaseType>>& syncedResources,
		DeducedArgs&&... args)
	{
		// Create resource in separate expression since no lock is required yet
		auto newResource = std::make_unique<DerivedType>(std::forward<DeducedArgs>(args)...);

		DerivedType* resourcePtr = nullptr;
		syncedResources.locked(
			[&resourcePtr, &newResource](auto& resources)
			{
				resourcePtr = resources.add(std::move(newResource));
			});

		return resourcePtr;
	}

	template<typename CookedType, typename... DeducedArgs>
	static CookedType* makeCookedResourceWithID(
		TSynchronized<TSdlResourceIdMap<CookedType>>& syncedIdToResource,
		const SdlResourceId id, 
		DeducedArgs&&... args)
	{
		// Create resource in separate expression since no lock is required yet
		auto newResource = std::make_unique<CookedType>(std::forward<DeducedArgs>(args)...);

		CookedType* resourcePtr = nullptr;
		syncedIdToResource.locked(
			[id, &resourcePtr, &newResource](auto& idToResource)
			{
				auto findResult = idToResource.find(id);
				if(findResult == idToResource.end())
				{
					resourcePtr = newResource.get();
					idToResource[id] = std::move(newResource);
				}
				else
				{
					PH_LOG(CookedResourceCollectionBase,
						"overwriting existing cooked resource (id: {})", id);

					// Clear the content of existing resource while keeping its pointer valid
					resourcePtr = findResult->second.get();
					*resourcePtr = std::move(*newResource);
				}
			});

		return resourcePtr;
	}

	template<typename CookedType, typename... DeducedArgs>
	static const CookedType* getCookedResourceByID(
		const TSynchronized<TSdlResourceIdMap<CookedType>>& syncedIdToResource,
		const SdlResourceId id)
	{
		const CookedType* resourcePtr = nullptr;
		syncedIdToResource.constLocked(
			[id, &resourcePtr](const auto& idToResource)
			{
				auto findResult = idToResource.find(id);
				if(findResult != idToResource.end())
				{
					resourcePtr = findResult->second.get();
				}
			});

		return resourcePtr;
	}
};

}// end namespace ph
