#pragma once

#include "StaticImageRenderer.h"

#include <ph_core.h>

#include <string>

namespace ph::cli
{

/*! @brief Renderer for working with Blender.
*/
class BlenderStaticImageRenderer : public StaticImageRenderer
{
public:
	explicit BlenderStaticImageRenderer(const ProcessedArguments& args);

	void render() override;
};

}// end namespace ph::cli
