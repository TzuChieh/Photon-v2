#include "SDL/SdlInputClause.h"
#include "SDL/SdlResourceLocator.h"

namespace ph
{

bool SdlInputClause::isResourceIdentifier() const
{
	return SdlResourceLocator::isRecognized(value);
}

}// end namespace ph
