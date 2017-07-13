#pragma once

#include "Utility/TIteratorProxy.h"
#include "Utility/TConstIteratorProxy.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/Intersectable/Intersectable.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/CookedActor.h"
#include "Math/Transform/Transform.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedActorStorage final
{
public:
	CookedActorStorage() = default;
	CookedActorStorage(CookedActorStorage&& other);
	~CookedActorStorage() = default;

	void clear();

	void add(std::unique_ptr<Intersectable> intersectable);
	void add(std::unique_ptr<PrimitiveMetadata> metadata);
	void add(std::unique_ptr<Emitter> emitter);
	void add(std::unique_ptr<Transform> transform);
	void add(CookedActor&& cookedActor);
	void add(std::vector<std::unique_ptr<Intersectable>>&& intersectables);
	void add(std::vector<std::unique_ptr<Transform>>&& transforms);
	void add(CookedActorStorage&& other);

	std::size_t numIntersectables() const;

	// forbid copying
	CookedActorStorage(const CookedActorStorage& other) = delete;
	CookedActorStorage& operator = (const CookedActorStorage& rhs) = delete;

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

private:
	std::vector<std::unique_ptr<Intersectable>>     m_intersectables;
	std::vector<std::unique_ptr<PrimitiveMetadata>> m_primitiveMetadatas;
	std::vector<std::unique_ptr<Emitter>>           m_emitters;
	std::vector<std::unique_ptr<Transform>>         m_transforms;
};

}// end namespace ph