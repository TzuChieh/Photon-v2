#pragma once

#include "Common/primitive_type.h"
#include "Core/Receiver/Receiver.h"
#include "World/VisualWorld.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "DataIO/SDL/NamedResourceStorage.h"
#include "Core/Renderer/Renderer.h"
#include "World/CookSettings.h"

#include <vector>
#include <memory>
#include <string>

namespace ph
{

class SdlResourcePack final
{
public:
	NamedResourceStorage data;

	SdlResourcePack();
};

}// end namespace ph
