#pragma once

#include "Core/Renderer.h"
#include "Core/SampleGenerator/PixelFixedSampleGenerator.h"

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>

namespace ph
{

class Film;
class Sample;

class PreviewRenderer final : public Renderer
{
public:
	PreviewRenderer(const uint32 numThreads);
	virtual ~PreviewRenderer() override;

	virtual void render(const World& world, const Camera& camera) const override;
	virtual float32 queryPercentageProgress() const override;
	virtual float32 querySampleFrequency() const override;

private:
	mutable std::vector<Film> m_subFilms;
	mutable std::mutex m_rendererMutex;

	std::vector<std::unique_ptr<std::atomic<float32>>> m_workerProgresses;
	std::vector<std::unique_ptr<std::atomic<float32>>> m_workerSampleFrequencies;

	// HACK
	PixelFixedSampleGenerator m_pixelFixedSampleGenerator;
};

}// end namespace ph