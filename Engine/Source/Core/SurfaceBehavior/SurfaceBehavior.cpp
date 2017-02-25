#include "Core/SurfaceBehavior/SurfaceBehavior.h"

#include <iostream>

namespace ph
{

SurfaceBehavior::SurfaceBehavior() : 
	m_bsdf(nullptr), m_emitter(nullptr)
{

}

void SurfaceBehavior::setBsdf(std::unique_ptr<BSDF> bsdf)
{
	if(!bsdf)
	{
		std::cerr << "warning: at SurfaceBehavior::setBsdf(), input is null" << std::endl;
	}

	m_bsdf = std::move(bsdf);
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