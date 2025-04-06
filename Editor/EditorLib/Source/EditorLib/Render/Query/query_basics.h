#pragma once

#include "EditorLib/EditorCore/Query/TQueryPerformer.h"
#include "EditorLib/EditorCore/Query/TQuery.h"

namespace ph::editor::render
{

class System;

using QueryPerformer = TQueryPerformer<System>;
using Query = TQuery<System>;

template<typename Performer>
using TQueryOf = TQuery<System, Performer>;

}// end namespace ph::editor::render
