#pragma once

#include <Common/primitive_type.h>

namespace ph::editor
{

/*!
Unless otherwise noted, unit is in pixel (raster coordinates). Origin is on the lower-left corner.
*/
struct DimensionHints final
{
	float32 fontSize = 15.0f;
	float32 largeIconSize = 25.0f;

	float32 propertyPanelPreferredWidth = 300.0f;
	float32 propertyPanelMinWidth = 100.0f;
	float32 propertyPanelMaxWidth = 450.0f;

	float32 mainViewportPreferredWidth = 1920.0f * 0.7f;
	float32 mainViewportPreferredHeight = 1080.0f * 0.7f;
	float32 mainViewportMinWidth = 400.0f;
	float32 mainViewportMinHeight = 300.0f;
	float32 mainViewportMaxWidth = 3840.0f;
	float32 mainViewportMaxHeight = 2160.0f;
};

}// end namespace ph::editor
