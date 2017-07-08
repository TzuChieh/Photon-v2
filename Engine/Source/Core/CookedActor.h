#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedActor final
{
public:
	std::vector<std::unique_ptr<Primitive>> primitives;
	std::unique_ptr<PrimitiveMetadata>      primitiveMetadata;
	std::unique_ptr<Emitter>                emitter;

	CookedActor();
	CookedActor(CookedActor&& other);
	~CookedActor() = default;

	// forbid copying
	CookedActor(const CookedActor& other) = delete;
	CookedActor& operator = (const CookedActor& rhs) = delete;

	CookedActor& operator = (CookedActor&& rhs);
};

}// end namespace ph