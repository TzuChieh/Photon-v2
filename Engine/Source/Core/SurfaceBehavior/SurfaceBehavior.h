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

	void setSurfaceOptics(std::unique_ptr<SurfaceOptics> optics);
	void setEmitter(const Emitter* emitter);

	inline const SurfaceOptics* getSurfaceOptics() const
	{
		return m_surfaceOptics.get();
	}

	inline const Emitter* getEmitter() const
	{
		return m_emitter;
	}

private:
	std::unique_ptr<SurfaceOptics> m_surfaceOptics;
	const Emitter* m_emitter;
};

}// end namespace ph