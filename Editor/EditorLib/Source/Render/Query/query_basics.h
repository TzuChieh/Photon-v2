#pragma once

#include "EditorCore/Query/TQueryPerformer.h"
#include "EditorCore/Query/TQuery.h"

namespace ph::editor
{
	class GraphicsContext;
}

namespace ph::editor::render
{

class System;

using QueryPerformer = TQueryPerformer<System>;
using Query = TQuery<System>;

}// end namespace ph::editor::render
