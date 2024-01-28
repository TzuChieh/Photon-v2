#pragma once

#include "Utility/TStableIndexDenseVector.h"
#include "Core/Engine.h"
#include "Frame/TFrame.h"
#include "Utility/ByteBuffer.h"

#include <Common/logging.h>

#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <string>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(ApiDatabase, Core);

class ApiDatabase final
{
public:
	template<typename Resource>
	static std::size_t addResource(std::unique_ptr<Resource> resource);

	template<typename Resource>
	static Resource* getResource(std::size_t id);

	template<typename Resource>
	static bool removeResource(std::size_t id);

	template<typename Resource>
	static std::weak_ptr<Resource> useResource(std::size_t id);

	static void clear();

private:
	template<typename Resource>
	static TStableIndexDenseVector<std::shared_ptr<Resource>>& RESOURCES();

	static std::mutex& MUTEX();
};

// In-header Implementations:

template<typename Resource>
inline std::size_t ApiDatabase::addResource(std::unique_ptr<Resource> resource)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	const std::size_t id = RESOURCES<Resource>().add(std::move(resource));

	PH_LOG(ApiDatabase, Note, "added resource<{}>", id);

	return id;
}

template<typename Resource>
inline Resource* ApiDatabase::getResource(const std::size_t id)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	auto* const resource = RESOURCES<Resource>().get(id);

	if(!resource)
	{
		PH_LOG(ApiDatabase, Warning, "resource<{}> does not exist", id);
	}

	return resource ? resource->get() : nullptr;
}

template<typename Resource>
inline bool ApiDatabase::removeResource(const std::size_t id)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	const bool isRemoved = RESOURCES<Resource>().remove(id);

	if(isRemoved)
	{
		PH_LOG(ApiDatabase, Note, "removed resource<{}>", id);
	}
	else
	{
		PH_LOG(ApiDatabase, Warning, "failed removing resource<{}>", id);
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
inline TStableIndexDenseVector<std::shared_ptr<Resource>>& ApiDatabase::RESOURCES()
{
	static_assert(
		std::is_same_v<Resource, Engine>      ||
		std::is_same_v<Resource, HdrRgbFrame> ||
		std::is_same_v<Resource, ByteBuffer>,
		"error: in ApiDatabase::getResource(), "
		"type of the specified resource is unsupported");

	static TStableIndexDenseVector<std::shared_ptr<Resource>> resources;
	return resources;
}

inline std::mutex& ApiDatabase::MUTEX()
{
	static std::mutex lock;
	return lock;
}

}// end namespace ph