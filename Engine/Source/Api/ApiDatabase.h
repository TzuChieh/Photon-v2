#pragma once

#include "Utility/TStableIndexDenseArray.h"
#include "Common/Logger.h"
#include "Core/Engine.h"
#include "Frame/TFrame.h"

#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <string>

namespace ph
{

class ApiDatabase final
{
public:
	template<typename Resource>
	static std::size_t addResource(std::unique_ptr<Resource> resource, Logger* logger = nullptr);

	template<typename Resource>
	static Resource* getResource(std::size_t id, Logger* logger = nullptr);

	template<typename Resource>
	static bool removeResource(std::size_t id, Logger* logger = nullptr);

	template<typename Resource>
	static std::weak_ptr<Resource> useResource(std::size_t id);

	static void clear();

private:
	template<typename Resource>
	static TStableIndexDenseArray<std::shared_ptr<Resource>>& RESOURCES();

	static std::mutex& MUTEX();
};

// In-header Implementations:

template<typename Resource>
inline std::size_t ApiDatabase::addResource(std::unique_ptr<Resource> resource, Logger* const logger)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	const std::size_t id = RESOURCES<Resource>().add(std::move(resource));

	if(logger)
	{
		logger->log(ELogLevel::NOTE_MED, 
			"added resource<" + std::to_string(id));
	}

	return id;
}

template<typename Resource>
inline Resource* ApiDatabase::getResource(const std::size_t id, Logger* const logger)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	auto* const resource = RESOURCES<Resource>().get(id);

	if(logger && !resource)
	{
		logger->log(ELogLevel::WARNING_MED,
			"resource<" + std::to_string(id) + "> does not exist");
	}

	return resource ? resource->get() : nullptr;
}

template<typename Resource>
inline bool ApiDatabase::removeResource(const std::size_t id, Logger* const logger)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	const bool isRemoved = RESOURCES<Resource>().remove(id);

	if(logger)
	{
		if(isRemoved)
		{
			logger->log(ELogLevel::NOTE_MED, 
				"removed resource<" + std::to_string(id));
		}
		else
		{
			logger->log(ELogLevel::WARNING_MED, 
				"failed removing resource<" + std::to_string(id) + ">");
		}
	}

	return isRemoved;
}

template<typename Resource>
inline std::weak_ptr<Resource> ApiDatabase::useResource(const std::size_t id)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return RESOURCES<Resource>().isStableIndexValid(id) ? RESOURCES<Resource>()[id] : nullptr;
}

template<typename Resource>
inline TStableIndexDenseArray<std::shared_ptr<Resource>>& ApiDatabase::RESOURCES()
{
	static_assert(
		std::is_same_v<Resource, Engine> ||
		std::is_same_v<Resource, HdrRgbFrame>,
		"error: in ApiDatabase::getResource(), "
		"type of the specified resource is unsupported");

	static TStableIndexDenseArray<std::shared_ptr<Resource>> resources;
	return resources;
}

inline std::mutex& ApiDatabase::MUTEX()
{
	static std::mutex lock;
	return lock;
}

}// end namespace ph