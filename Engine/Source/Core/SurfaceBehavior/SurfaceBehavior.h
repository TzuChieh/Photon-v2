#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"
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
	void setEmitter(std::unique_ptr<Emitter> emitter);

	inline const BSDFcos* getBsdfCos() const
	{
		return m_bsdfCos.get();
	}

	inline const Emitter* getEmitter() const
	{
		return m_emitter.get();
	}

private:
	std::unique_ptr<BSDFcos> m_bsdfCos;
	std::unique_ptr<Emitter> m_emitter;
};

}// end namespace ph