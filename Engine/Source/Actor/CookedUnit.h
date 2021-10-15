#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"
#include "Math/Transform/Transform.h"
#include "Utility/IMoveOnly.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedDataStorage;
class Primitive;

class CookedUnit final : private IMoveOnly
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
	const std::vector<std::unique_ptr<Intersectable>>& intersectables() const
	{
		return m_intersectables;
	}

	void addIntersectable(std::unique_ptr<Intersectable> intersectable);
	void setPrimitiveMetadata(std::unique_ptr<PrimitiveMetadata> metadata);
	void addTransform(std::unique_ptr<math::Transform> transform);
	void setEmitter(std::unique_ptr<Emitter> emitter);

	void addBackend(std::unique_ptr<Intersectable> intersectable);

	void claimCookedData(CookedDataStorage& storage);
	void claimCookedBackend(CookedDataStorage& storage);

	CookedUnit& operator = (CookedUnit&& rhs);

private:
	std::vector<std::unique_ptr<Intersectable>>   m_intersectables;
	std::unique_ptr<PrimitiveMetadata>            m_primitiveMetadata;
	std::vector<std::unique_ptr<math::Transform>> m_transforms;
	std::unique_ptr<Emitter>                      m_emitter;
	std::vector<std::unique_ptr<Intersectable>>   m_backendIntersectables;
};

}// end namespace ph
