#pragma once

#include <Common/primitive_type.h>

namespace ph::editor
{

/*!
Unless otherwise noted, unit is in pixel (raster coordinates). Origin is on the lower-left corner.
*/
class DimensionHints final
{
public:
	float32 fontSize;
	float32 largeFontSize;

	float32 propertyPanelPreferredWidth;
	float32 propertyPanelMinWidth;
	float32 propertyPanelMaxWidth;

	float32 mainViewportPreferredWidth;
	float32 mainViewportPreferredHeight;
	float32 mainViewportMinWidth;
	float32 mainViewportMinHeight;
	float32 mainViewportMaxWidth;
	float32 mainViewportMaxHeight;

public:
	DimensionHints();

	void applyDpiScale(float32 dpiScale);
};

}// end namespace ph::editor
