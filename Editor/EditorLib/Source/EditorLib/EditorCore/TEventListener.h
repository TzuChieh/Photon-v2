#pragma once

#include <Engine/Utility/TFunction.h>

namespace ph::editor
{

template<typename EventType>
using TEventListener = TFunction<void(const EventType& e)>;

}// end namespace ph::editor
