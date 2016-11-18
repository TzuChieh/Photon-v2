#pragma once

#include "Core/Renderer.h"

#include <vector>
#include <mutex>

namespace ph
{

class Film;
class Sample;

class MtImportanceRenderer : public Renderer
{
public:
	virtual ~MtImportanceRenderer() override;

	virtual void render(const World& world, const Camera& camera) const override;

private:
	mutable std::mutex m_mutex;
};

}// end namespace ph