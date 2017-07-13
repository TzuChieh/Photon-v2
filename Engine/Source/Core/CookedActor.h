#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"
#include "Math/Transform/Transform.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedActor final
{
public:
	std::vector<std::unique_ptr<Intersectable>> intersectables;
	std::unique_ptr<PrimitiveMetadata>          primitiveMetadata;
	std::unique_ptr<Emitter>                    emitter;
	std::vector<std::unique_ptr<Transform>>     transforms;

	CookedActor();
	CookedActor(CookedActor&& other);
	~CookedActor() = default;

	CookedActor& operator = (CookedActor&& rhs);

	// forbid copying
	CookedActor(const CookedActor& other) = delete;
	CookedActor& operator = (const CookedActor& rhs) = delete;
};

}// end namespace ph