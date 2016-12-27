#pragma once

#include "Utility/TConstIteratorProxy.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/Primitive/Primitive.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Math/Transform.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedModelStorage final
{
public:
	CookedModelStorage() = default;
	~CookedModelStorage() = default;

	void clear();

	void add(std::unique_ptr<Primitive> primitive);
	void add(std::unique_ptr<PrimitiveMetadata> metadata);
	void add(std::unique_ptr<Transform> transform);
	void add(std::unique_ptr<SurfaceBehavior> surfaceBehavior);
	void add(std::vector<std::unique_ptr<Primitive>>&& primitives);

	void add(CookedModelStorage&& other);

	// forbid copying
	CookedModelStorage(const CookedModelStorage& other) = delete;
	CookedModelStorage& operator = (const CookedModelStorage& rhs) = delete;

	inline TConstIteratorProxy<std::vector<std::unique_ptr<Primitive>>> primitives() const
	{
		return TConstIteratorProxy<std::vector<std::unique_ptr<Primitive>>>(m_primitives);
	}

	std::size_t numPrimitives() const;

private:
	std::vector<std::unique_ptr<Primitive>>         m_primitives;
	std::vector<std::unique_ptr<PrimitiveMetadata>> m_primitiveMetadatas;
	std::vector<std::unique_ptr<Transform>>         m_transforms;
	std::vector<std::unique_ptr<SurfaceBehavior>>   m_surfaceBehaviors;
};

}// end namespace ph