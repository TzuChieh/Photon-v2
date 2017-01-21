#pragma once

#include "Common/primitive_type.h"
#include "Actor/Actor.h"

#include <vector>
#include <memory>
#include <string>

namespace ph
{

class RenderingDescription final
{
public:
	std::string cameraType;
	float32 cameraFov;
	uint32 filmWidthPx;
	uint32 filmHeightPx;
	std::vector<std::unique_ptr<Actor>>& actors;
};

}// end namespace ph