#pragma once

namespace ph
{

class ExponentialTransmittance;

class SceneGlobals final
{
public:
	static const ExponentialTransmittance* getExponentialTransmittance();
};

}// end namespace ph
