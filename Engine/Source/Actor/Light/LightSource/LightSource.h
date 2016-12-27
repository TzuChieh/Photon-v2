#pragma once

namespace ph
{

class PrimitiveStorage;
class EmitterStorage;
class EmitterMetadata;
class Model;

class LightSource
{
public:
	virtual ~LightSource() = 0;

	virtual void buildEmitters(PrimitiveStorage* const out_primitiveStorage, 
	                           EmitterStorage* const out_emitterStorage, 
	                           const Model& lightModel) const = 0;
};

}// end namespace ph