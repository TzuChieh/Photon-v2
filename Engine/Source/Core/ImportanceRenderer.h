#pragma once

#include "Core/Renderer.h"

namespace ph
{

class ImportanceRenderer final : public Renderer
{
public:
	virtual ~ImportanceRenderer() override;

	virtual void render(const World& world, const Camera& camera) const override;
	virtual void queryIntermediateFilm(Film* const out_film) const override;
	virtual float32 queryPercentageProgress() const override;
};

}// end namespace ph