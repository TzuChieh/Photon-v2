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
	MtImportanceRenderer(const uint32 numThreads);
	virtual ~MtImportanceRenderer() override;

	virtual void render(const World& world, const Camera& camera) const override;
	virtual float32 queryPercentageProgress() const override;
	virtual float32 querySampleFrequency() const override;

private:
	mutable std::vector<Film> m_subFilms;
	mutable std::mutex m_rendererMutex;

	std::vector<std::unique_ptr<std::atomic<float32>>> m_workerProgresses;
	std::vector<std::unique_ptr<std::atomic<float32>>> m_workerSampleFrequencies;
};

}// end namespace ph