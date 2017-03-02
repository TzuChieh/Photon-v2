#pragma once

#include "Utility/TIteratorProxy.h"
#include "Utility/TConstIteratorProxy.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/Primitive/Primitive.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Math/Transform.h"
#include "Core/CookedActor.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedActorStorage final
{
public:
	CookedActorStorage() = default;
	~CookedActorStorage() = default;

	void clear();

	void add(std::unique_ptr<Primitive> primitive);
	void add(std::unique_ptr<PrimitiveMetadata> metadata);
	void add(std::unique_ptr<Emitter> emitter);
	void add(CookedActor&& cookedActor);
	void add(std::vector<std::unique_ptr<Primitive>>&& primitives);
	void add(CookedActorStorage&& other);

	std::size_t numPrimitives() const;

	// forbid copying
	CookedActorStorage(const CookedActorStorage& other) = delete;
	CookedActorStorage& operator = (const CookedActorStorage& rhs) = delete;

	inline TIteratorProxy<std::vector<std::unique_ptr<Primitive>>> primitives()
	{
		return TIteratorProxy<std::vector<std::unique_ptr<Primitive>>>(m_primitives);
	}

	inline TConstIteratorProxy<std::vector<std::unique_ptr<Primitive>>> primitives() const
	{
		return TConstIteratorProxy<std::vector<std::unique_ptr<Primitive>>>(m_primitives);
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
	std::vector<std::unique_ptr<Primitive>>         m_primitives;
	std::vector<std::unique_ptr<PrimitiveMetadata>> m_primitiveMetadatas;
	std::vector<std::unique_ptr<Emitter>>           m_emitters;
};

}// end namespace ph