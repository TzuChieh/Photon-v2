#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/BSDF.h"
#include "Core/Emitter/Emitter.h"

#include <memory>

namespace ph
{

class SurfaceBehavior final
{
public:
	SurfaceBehavior();

	void setBsdf(std::unique_ptr<BSDF> bsdf);
	void setEmitter(const Emitter* emitter);

	inline const BSDF* getBsdf() const
	{
		return m_bsdf.get();
	}

	inline const Emitter* getEmitter() const
	{
		return m_emitter;
	}

private:
	std::unique_ptr<BSDF> m_bsdf;
	const Emitter* m_emitter;
};

}// end namespace ph