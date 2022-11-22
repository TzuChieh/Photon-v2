#include "App/DimensionHints.h"

namespace ph::editor
{

DimensionHints::DimensionHints()
	: fontSize(15.0f)
	, largeFontSize(40.0f)

	, propertyPanelPreferredWidth(fontSize * 15.0f)
	, propertyPanelMinWidth(propertyPanelPreferredWidth * 0.333f)
	, propertyPanelMaxWidth(propertyPanelPreferredWidth * 3.0f)

	, mainViewportPreferredWidth(1920.0f * 0.7f)
	, mainViewportPreferredHeight(1080.0f * 0.7f)
	, mainViewportMinWidth(400.0f)
	, mainViewportMinHeight(300.0f)
	, mainViewportMaxWidth(3840.0f)
	, mainViewportMaxHeight(2160.0f)
{}

void DimensionHints::applyDpiScale(const float32 dpiScale)
{
	// Not all dimensions should respond to DPI scale, e.g., images and 3D contents.

	fontSize *= dpiScale;
	largeFontSize *= dpiScale;

	propertyPanelPreferredWidth *= dpiScale;
	propertyPanelMinWidth *= dpiScale;
	propertyPanelMaxWidth *= dpiScale;
}

}// end namespace ph::editor
