#pragma once

#include "Utility/TIteratorProxy.h"
#include "Utility/TConstIteratorProxy.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/Intersection/Intersectable.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "World/Foundation/TransientVisualElement.h"
#include "Math/Transform/Transform.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedDataStorage final
{
public:
	CookedDataStorage() = default;
	CookedDataStorage(CookedDataStorage&& other);
	~CookedDataStorage() = default;

	void clear();

	void add(std::unique_ptr<Intersectable> intersectable);
	void add(std::unique_ptr<PrimitiveMetadata> metadata);
	void add(std::unique_ptr<Emitter> emitter);
	void add(std::unique_ptr<math::Transform> transform);
	void add(std::vector<std::unique_ptr<Intersectable>>&& intersectables);
	void add(std::vector<std::unique_ptr<math::Transform>>&& transforms);
	void add(CookedDataStorage&& other);

	std::size_t numIntersectables() const;
	std::size_t numEmitters() const;

	inline TIteratorProxy<std::vector<std::unique_ptr<Intersectable>>> intersectables()
	{
		return TIteratorProxy<std::vector<std::unique_ptr<Intersectable>>>(m_intersectables);
	}

	inline TConstIteratorProxy<std::vector<std::unique_ptr<Intersectable>>> intersectables() const
	{
		return TConstIteratorProxy<std::vector<std::unique_ptr<Intersectable>>>(m_intersectables);
	}

	inline TIteratorProxy<std::vector<std::unique_ptr<PrimitiveMetadata>>> primitiveMetadatas()
	{
		return TIteratorProxy<std::vector<std::unique_ptr<PrimitiveMetadata>>>(m_primitiveMetadatas);
	}

	inline TConstIteratorProxy<std::vector<std::unique_ptr<PrimitiveMetadata>>> primitiveMetadatas() const
	{
		return TConstIteratorProxy<std::vector<std::unique_ptr<PrimitiveMetadata>>>(m_primitiveMetadatas);
	}

	inline TIteratorProxy<std::vector<std::unique_ptr<Emitter>>> emitters()
	{
		return TIteratorProxy<std::vector<std::unique_ptr<Emitter>>>(m_emitters);
	}

	inline TConstIteratorProxy<std::vector<std::unique_ptr<Emitter>>> emitters() const
	{
		return TConstIteratorProxy<std::vector<std::unique_ptr<Emitter>>>(m_emitters);
	}

	// forbid copying
	//
	CookedDataStorage(const CookedDataStorage& other) = delete;
	CookedDataStorage& operator = (const CookedDataStorage& rhs) = delete;

private:
	std::vector<std::unique_ptr<Intersectable>>     m_intersectables;
	std::vector<std::unique_ptr<PrimitiveMetadata>> m_primitiveMetadatas;
	std::vector<std::unique_ptr<Emitter>>           m_emitters;
	std::vector<std::unique_ptr<math::Transform>>   m_transforms;
};

}// end namespace ph
