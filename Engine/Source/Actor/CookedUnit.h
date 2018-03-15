#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"
#include "Math/Transform/Transform.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedUnit final
{
public:
	std::vector<std::unique_ptr<Intersectable>>     intersectables;
	std::vector<std::unique_ptr<PrimitiveMetadata>> primitiveMetadatas;
	std::vector<std::unique_ptr<Emitter>>           emitters;
	std::vector<std::unique_ptr<Transform>>         transforms;

	CookedUnit();
	CookedUnit(CookedUnit&& other);
	~CookedUnit() = default;

	CookedUnit& operator = (CookedUnit&& rhs);

	// forbid copying
	CookedUnit(const CookedUnit& other) = delete;
	CookedUnit& operator = (const CookedUnit& rhs) = delete;
};

}// end namespace ph