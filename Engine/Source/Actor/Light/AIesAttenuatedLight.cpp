#include "Actor/Light/AIesAttenuatedLight.h"

namespace ph
{

void AIesAttenuatedLight::setSource(const std::shared_ptr<ALight>& source)
{
	m_source = source;
}

void AIesAttenuatedLight::setIesFile(const Path& iesFile)
{
	m_iesFile.setPath(iesFile);
}

}// end namespace ph
