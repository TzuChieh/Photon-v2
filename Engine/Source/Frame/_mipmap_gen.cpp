#include "Frame/_mipmap_gen.h"

namespace ph
{

mipmapgen::mipmapgen(const std::size_t numThreads) :
	m_workers(numThreads)
{

}

}// end namespace ph