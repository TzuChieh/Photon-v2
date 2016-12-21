#include "Emitter/SurfaceEmitter.h"
#include "Math/Vector3f.h"
#include "Entity/Geometry/Geometry.h"

namespace ph
{

SurfaceEmitter::SurfaceEmitter(const std::shared_ptr<Geometry>& geometry) : 
	m_geometry(geometry)
{

}

SurfaceEmitter::~SurfaceEmitter()
{

}

void SurfaceEmitter::samplePosition(const Vector3f& position, const Vector3f& direction, float32* const out_PDF, Vector3f* const out_emittedRadiance) const
{

}

}// end namespace ph