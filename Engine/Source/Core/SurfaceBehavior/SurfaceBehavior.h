#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Emitter/Emitter.h"

#include <memory>

namespace ph
{

class SurfaceBehavior final
{
public:
	SurfaceBehavior();

	void setOptics(const std::shared_ptr<SurfaceOptics>& optics);
	void setEmitter(const Emitter* emitter);
	std::shared_ptr<SurfaceOptics> getOpticsResource() const;

	inline const SurfaceOptics* getOptics() const
	{
		return m_optics.get();
	}

	inline const Emitter* getEmitter() const
	{
		return m_emitter;
	}

private:
	std::shared_ptr<SurfaceOptics> m_optics;

	// FIXME: store SurfaceEmitter only
	const Emitter*                 m_emitter;
};

}// end namespace ph