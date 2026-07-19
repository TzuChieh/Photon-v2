#pragma once

#include "Engine/SDL/SdlResourceId.h"
#include "Engine/Math/hash.h"
#include "Engine/World/Foundation/CommonCookingConfig.h"
#include "Engine/World/Foundation/GeometryCookingConfig.h"
#include "Engine/World/Foundation/MaterialCookingConfig.h"
#include "Engine/World/Foundation/MotionCookingConfig.h"

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

namespace ph
{

template<typename ResourceConfig>
class TCookedResourceKey final
{
public:
	SdlResourceId id = EMPTY_SDL_RESOURCE_ID;
	CommonCookingConfig commonConfig;
	ResourceConfig resourceConfig;

	TCookedResourceKey() = default;

	TCookedResourceKey(
		const SdlResourceId id,
		CommonCookingConfig commonConfig,
		ResourceConfig resourceConfig)
		: id(id)
		, commonConfig(std::move(commonConfig))
		, resourceConfig(std::move(resourceConfig))
	{
		// Should be trivially copyable so there are no surprises when being hashed
		static_assert(std::is_trivially_copyable_v<TCookedResourceKey>);
	}

	friend bool operator == (const TCookedResourceKey& lhs, const TCookedResourceKey& rhs) = default;
};

using CookedGeometryKey = TCookedResourceKey<GeometryCookingConfig>;
using CookedMaterialKey = TCookedResourceKey<MaterialCookingConfig>;
using CookedMotionKey   = TCookedResourceKey<MotionCookingConfig>;

}// end namespace ph

namespace std
{

template<typename ResourceConfig>
struct hash<ph::TCookedResourceKey<ResourceConfig>>
{
	std::size_t operator () (const ph::TCookedResourceKey<ResourceConfig>& key) const
	{
		std::size_t hash = std::hash<ph::SdlResourceId>{}(key.id);
		hash = ph::math::combine_hashes(
			hash, std::hash<ph::CommonCookingConfig>{}(key.commonConfig));
		hash = ph::math::combine_hashes(
			hash, std::hash<ResourceConfig>{}(key.resourceConfig));
		return hash;
	}
};

}// end namespace std
