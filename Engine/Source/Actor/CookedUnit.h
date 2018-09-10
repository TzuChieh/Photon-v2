#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"
#include "Math/Transform/Transform.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedDataStorage;
class Primitive;

class CookedUnit final
{
public:
	CookedUnit();
	CookedUnit(CookedUnit&& other);
	~CookedUnit() = default;

	// HACK: use begin/end instead
	std::vector<std::unique_ptr<Intersectable>>& intersectables()
	{
		return m_intersectables;
	}

	void addIntersectable(std::unique_ptr<Intersectable> intersectable);
	void setPrimitiveMetadata(std::unique_ptr<PrimitiveMetadata> metadata);
	void addTransform(std::unique_ptr<Transform> transform);
	void setEmitter(std::unique_ptr<Emitter> emitter);

	void addBackend(std::unique_ptr<Intersectable> intersectable);

	void claimCookedData(CookedDataStorage& storage);
	void claimCookedBackend(CookedDataStorage& storage);

	CookedUnit& operator = (CookedUnit&& rhs);

	// forbid copying
	//
	CookedUnit(const CookedUnit& other) = delete;
	CookedUnit& operator = (const CookedUnit& rhs) = delete;

	// HACK
	bool isBackgroundEmitter() const
	{
		return m_backgroundEmitterPrimitive;
	}

	// HACK
	void setBackgroundEmitterPrimitive(const Primitive* primitive)
	{
		m_backgroundEmitterPrimitive = primitive;
	}

private:
	std::vector<std::unique_ptr<Intersectable>> m_intersectables;
	std::unique_ptr<PrimitiveMetadata>          m_primitiveMetadata;
	std::vector<std::unique_ptr<Transform>>     m_transforms;
	std::unique_ptr<Emitter>                    m_emitter;
	std::vector<std::unique_ptr<Intersectable>> m_backendIntersectables;

	// HACK (use ECS tagging or other methods)
	const Primitive* m_backgroundEmitterPrimitive;
};

}// end namespace ph