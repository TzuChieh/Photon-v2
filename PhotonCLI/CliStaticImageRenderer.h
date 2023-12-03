#pragma once

#include "StaticImageRenderer.h"

namespace ph::cli
{

class CliStaticImageRenderer : public StaticImageRenderer
{
public:
	explicit CliStaticImageRenderer(const ProcessedArguments& args);

	void render() override;
};

}// end namespace ph::cli
