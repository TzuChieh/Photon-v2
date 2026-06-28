#pragma once

#include "Engine/SDL/ESdlTypeCategory.h"
#include "Engine/SDL/ISdlResource.h"
#include "Engine/SDL/SdlResourceId.h"
#include "Engine/Math/hash.h"
#include "Engine/World/Foundation/CookingConfig.h"

#include <cstddef>
#include <functional>
#include <type_traits>

namespace ph
{

class CookedResourceKey final
{
public:
	ESdlTypeCategory category = ESdlTypeCategory::Unspecified;
	SdlResourceId id = EMPTY_SDL_RESOURCE_ID;
	CookingConfig config;
	
	CookedResourceKey() = default;
	CookedResourceKey(const ISdlResource& resource, const CookingConfig& cfg);

	friend bool operator == (const CookedResourceKey& lhs, const CookedResourceKey& rhs)
	{
		return lhs.category == rhs.category &&
		       lhs.id == rhs.id &&
		       lhs.config == rhs.config;
	}
};

inline CookedResourceKey::CookedResourceKey(
	const ISdlResource& resource,
	const CookingConfig& cfg)
	: category(resource.getDynamicCategory())
	, id(resource.getId())
	, config(cfg)
{}

// Should be trivially copyable so there are no surprises when being hashed
static_assert(std::is_trivially_copyable_v<CookedResourceKey>);

}// end namespace ph

namespace std
{

template<>
struct hash<ph::CookedResourceKey>
{
	std::size_t operator () (const ph::CookedResourceKey& key) const
	{
		std::size_t hash = std::hash<ph::ESdlTypeCategory>{}(key.category);
		hash = ph::math::combine_hashes(hash, std::hash<ph::SdlResourceId>{}(key.id));
		hash = ph::math::combine_hashes(hash, std::hash<ph::CookingConfig>{}(key.config));
		return hash;
	}
};

}// end namespace std
