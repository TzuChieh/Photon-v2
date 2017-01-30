#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Core/Emitter/Emitter.h"

#include <memory>

namespace ph
{

class SurfaceBehavior final
{
public:
	SurfaceBehavior();

	void setBsdfCos(std::unique_ptr<BSDFcos> bsdfCos);
	void setEmitter(const Emitter* emitter);

	inline const BSDFcos* getBsdfCos() const
	{
		return m_bsdfCos.get();
	}

	inline const Emitter* getEmitter() const
	{
		return m_emitter;
	}

private:
	std::unique_ptr<BSDFcos> m_bsdfCos;
	const Emitter* m_emitter;
};

}// end namespace ph