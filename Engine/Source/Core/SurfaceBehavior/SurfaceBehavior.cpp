#include "Core/SurfaceBehavior/SurfaceBehavior.h"

#include <iostream>

namespace ph
{

SurfaceBehavior::SurfaceBehavior() : 
	m_bsdfCos(nullptr), m_emitter(nullptr)
{

}

void SurfaceBehavior::setBsdfCos(std::unique_ptr<BSDFcos> bsdfCos)
{
	if(!bsdfCos)
	{
		std::cerr << "warning: at SurfaceBehavior::setBsdfCos(), input is null" << std::endl;
	}

	m_bsdfCos = std::move(bsdfCos);
}

void SurfaceBehavior::setEmitter(const Emitter* emitter)
{
	if(!emitter)
	{
		std::cerr << "warning: at SurfaceBehavior::setEmitter(), input is null" << std::endl;
	}

	m_emitter = emitter;
}

}// end namespace ph