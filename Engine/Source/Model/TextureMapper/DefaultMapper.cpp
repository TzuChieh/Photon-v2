#include "Model/TextureMapper/DefaultMapper.h"
#include "Math/Vector3f.h"

namespace ph
{

DefaultMapper::~DefaultMapper() = default;

void DefaultMapper::map(const Vector3f& position, const float32 u, const float32 v, float32* const out_u, float32* const out_v) const
{
	*out_u = u;
	*out_v = v;
}

}// end namespace ph