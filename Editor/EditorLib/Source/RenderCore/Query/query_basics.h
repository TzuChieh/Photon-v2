#pragma once

#include "EditorCore/Query/TQueryPerformer.h"
#include "EditorCore/Query/TQuery.h"

namespace ph::editor
{
	class GraphicsContext;
}

namespace ph::editor::ghi
{

using QueryPerformer = TQueryPerformer<GraphicsContext>;
using Query = TQuery<GraphicsContext>;

}// end namespace ph::editor::ghi
