#pragma once

#include "Core/Renderer/RenderWork.h"

#include <cstddef>

namespace ph
{

class PhotonMappingWork : public RenderWork
{
public:
	PhotonMappingWork(std::size_t numPhotons);
	void doWork() override;

private:
	std::size_t m_numPhotons;
};

}// end namespace ph