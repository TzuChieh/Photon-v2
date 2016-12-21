#pragma once

#include "Core/Emitter/Emitter.h"

#include <memory>

namespace ph
{

class Primitive;

class PrimitiveEmitter final : public Emitter
{
public:
	PrimitiveEmitter(const EmitterMetadata* const metadata, const Primitive* const primitive);
	virtual ~PrimitiveEmitter() override;

	virtual void samplePosition(const Vector3f& position, const Vector3f& direction, float32* const out_PDF, Vector3f* const out_emittedRadiance) const override;

private:
	const Primitive* m_primitive;
};

}// end namespace ph