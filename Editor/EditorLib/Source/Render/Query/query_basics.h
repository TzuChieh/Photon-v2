#pragma once

#include "EditorCore/Query/TQueryPerformer.h"
#include "EditorCore/Query/TQuery.h"

namespace ph::editor::render
{

class System;

using QueryPerformer = TQueryPerformer<System>;
using Query = TQuery<System>;

template<typename Performer>
using TQueryOf = TQuery<System, Performer>;

}// end namespace ph::editor::render
