#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/Emitter/Emitter.h"

#include <Common/primitive_type.h>

namespace ph
{

class SurfaceBehavior final
{
public:
	SurfaceBehavior();

	void setOptics(const SurfaceOptics* optics);
	void setEmitter(const Emitter* emitter);

	const SurfaceOptics* getOptics() const;
	const Emitter* getEmitter() const;

private:
	const SurfaceOptics* m_optics;

	// FIXME: store SurfaceEmitter only
	const Emitter* m_emitter;
};

inline const SurfaceOptics* SurfaceBehavior::getOptics() const
{
	return m_optics;
}

inline const Emitter* SurfaceBehavior::getEmitter() const
{
	return m_emitter;
}

}// end namespace ph
