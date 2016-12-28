#pragma once

#include "Core/Primitive/Primitive.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"

#include <vector>
#include <memory>

namespace ph
{

class CoreActor final
{
public:
	std::vector<std::unique_ptr<Primitive>> primitives;
	std::unique_ptr<PrimitiveMetadata>      primitiveMetadata;
	std::unique_ptr<Emitter>                emitter;

	CoreActor();
	CoreActor(CoreActor&& other);
	~CoreActor() = default;

	// forbid copying
	CoreActor(const CoreActor& other) = delete;
	CoreActor& operator = (const CoreActor& rhs) = delete;

	CoreActor& operator = (CoreActor&& rhs);
};

}// end namespace ph