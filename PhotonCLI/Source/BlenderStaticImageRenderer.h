#pragma once

#include "StaticImageRenderer.h"

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
