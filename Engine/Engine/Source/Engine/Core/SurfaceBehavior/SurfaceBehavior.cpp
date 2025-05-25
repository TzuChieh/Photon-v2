#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/World/SceneGlobals.h"

namespace ph
{

SurfaceBehavior::SurfaceBehavior()
	: m_optics(nullptr)
	, m_emitter(nullptr)
	, m_isObstructive(false)
	, m_isEmissive(false)
{
	setOptics(nullptr);
	setEmitter(nullptr);
}

void SurfaceBehavior::setOptics(const SurfaceOptics* optics)
{
	if(optics)
	{
		m_optics = optics;
		m_isObstructive = true;
	}
	else
	{
		m_optics = SceneGlobals::getFullyTransmissiveSurfaceOptics();
		m_isObstructive = false;
	}
}

void SurfaceBehavior::setEmitter(const SurfaceEmitter* emitter)
{
	if(emitter)
	{
		m_emitter = emitter;
		m_isEmissive = true;
	}
	else
	{
		m_emitter = SceneGlobals::getZeroSurfaceEmitter();
		m_isEmissive = false;
	}
}

}// end namespace ph
