#pragma once

#include "Core/Renderer.h"

#include <vector>
#include <mutex>
#include <memory>

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

	virtual void queryIntermediateFilm(Film* const out_film) const override;
	virtual float32 queryPercentageProgress() const override;

private:
	mutable std::vector<Film> m_subFilms;

	mutable std::vector<std::unique_ptr<std::mutex>> m_renderWorkerMutices;
	mutable std::mutex m_rendererMutex;
};

}// end namespace ph