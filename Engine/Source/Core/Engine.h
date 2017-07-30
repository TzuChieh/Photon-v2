#pragma once

#include "World/VisualWorld.h"
#include "Core/Renderer/Renderer.h"
#include "FileIO/DescriptionParser.h"
#include "FileIO/Description.h"
#include "Math/TVector2.h"

#include <string>

namespace ph
{

class Frame;

class Engine final
{
public:
	//VisualWorld visualWorld;

	Engine();

	void enterCommand(const std::string& commandFragment);
	void render();
	void developFilm(Frame& out_frame);
	TVector2<int64> getFilmDimensionPx() const;
	void setNumRenderThreads(const std::size_t numThreads);

	float32 queryPercentageProgress() const;
	float32 querySampleFrequency() const;

private:
	DescriptionParser m_parser;
	Description m_description;
	Renderer m_renderer;
};

}// end namespace ph