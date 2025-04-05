#include "Engine/SDL/SdlInputClause.h"
#include "Engine/SDL/SdlResourceLocator.h"

namespace ph
{

bool SdlInputClause::isResourceIdentifier() const
{
	return SdlResourceLocator::isRecognized(value);
}

}// end namespace ph
