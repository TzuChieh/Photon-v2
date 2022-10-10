#include "App/Event/Event.h"

#include <Utility/TBitFlags.h>

namespace ph::editor
{

bool Event::isFromSource(const EEventSource fromSourceType) const
{
	return TEnumFlags<EEventSource>({getSourceType()}).hasAny({fromSourceType});
}

}// end namespace ph::editor
