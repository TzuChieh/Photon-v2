#pragma once

#include "Utility/INoCopyAndMove.h"
#include "Utility/TUniquePtrVector.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/Transform.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "Utility/traits.h"

#include <vector>
#include <utility>
#include <memory>

namespace ph
{

/*! @brief Provides thread-safe cooked data creation and storage.
*/
class CookedResourceCollection final : private INoCopyAndMove
{
public:
	template<typename... DeducedArgs>
	PrimitiveMetadata* makeMetadata(DeducedArgs&&... args)
	{
		// Create metadata in separate expression since no lock is required yet
		auto newMetadata = std::make_unique<PrimitiveMetadata>(std::forward<DeducedArgs>(args)...);

		return m_metadatas->add(std::move(newMetadata));
	}

	template<CDerived<math::Transform> TransformType, typename... DeducedArgs>
	TransformType* makeTransform(DeducedArgs&&... args)
	{
		// Create transform in separate expression since no lock is required yet
		auto newTransform = std::make_unique<TransformType>(std::forward<DeducedArgs>(args)...);

		return m_transforms->add(std::move(newTransform));
	}

private:
	TSynchronized<TUniquePtrVector<PrimitiveMetadata>> m_metadatas;
	TSynchronized<TUniquePtrVector<math::Transform>> m_transforms;
};

}// end namespace ph
