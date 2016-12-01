#pragma once

#include "Core/Renderer.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>

namespace ph
{

class Film;
class Sample;

class MtImportanceRenderer final : public Renderer
{
public:
	MtImportanceRenderer();
	virtual ~MtImportanceRenderer() override;

	virtual void render(const World& world, const Camera& camera) const override;
	virtual float32 queryPercentageProgress() const override;

private:
	mutable std::vector<Film> m_subFilms;
	mutable std::vector<std::unique_ptr<std::atomic<float32>>> m_workerProgresses;
	mutable std::mutex m_rendererMutex;

	static const uint32 nThreads = 4;
};

}// end namespace ph