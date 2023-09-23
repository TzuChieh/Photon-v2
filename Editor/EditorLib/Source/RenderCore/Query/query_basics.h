#pragma once

#include "EditorCore/Query/TQueryPerformer.h"
#include "EditorCore/Query/TQuery.h"

namespace ph::editor::ghi
{

class GraphicsContext;

using QueryPerformer = TQueryPerformer<GraphicsContext>;
using Query = TQuery<GraphicsContext>;

template<typename Performer>
using TQueryOf = TQuery<GraphicsContext, Performer>;

}// end namespace ph::editor::ghi
