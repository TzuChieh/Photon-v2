#pragma once

namespace ph
{

class CookedModelStorage;
class CookedLightStorage;
class EmitterMetadata;
class Model;

class LightSource
{
public:
	virtual ~LightSource() = 0;

	virtual void buildEmitters(CookedModelStorage* const out_cookedModelStorage,
	                           CookedLightStorage* const out_cookedLightStorage,
	                           const Model& lightModel) const = 0;
};

}// end namespace ph