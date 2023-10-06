#pragma once

#include <Common/primitive_type.h>
#include <Frame/Viewport.h>

#include <string>
#include <vector>

namespace ph::editor::render
{

enum class EOfflineRenderStage
{
	Standby = 0,
	CopyingScene,
	LoadingScene,
	Updating,
	Rendering,
	Developing,
	Finished
};

class OfflineRenderStats final
{
public:
	struct NumericInfo
	{
		std::string name;
		float64 value = 0.0;
		bool isInteger = false;
	};

	Viewport viewport;
	uint64 totalWork = 0;
	uint64 workDone = 0;
	std::vector<std::string> layerNames;
	std::vector<NumericInfo> numericInfos;
};

}// end namespace ph::editor::render
