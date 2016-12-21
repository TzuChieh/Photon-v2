#pragma once

#include "Core/Emitter/Emitter.h"
#include "Core/Emitter/EmitterMetadata.h"
#include "Core/Primitive/PrimitiveStorage.h"

#include <vector>
#include <memory>

namespace ph
{

class EmitterStorage final
{
public:
	void clear();
	void add(std::unique_ptr<Emitter> emitter);
	void add(std::unique_ptr<EmitterMetadata> metadata);
	void add(PrimitiveStorage&& primitives);

	typename std::vector<std::unique_ptr<Emitter>>::iterator       begin() noexcept;
	typename std::vector<std::unique_ptr<Emitter>>::const_iterator begin() const noexcept;
	typename std::vector<std::unique_ptr<Emitter>>::iterator       end()   noexcept;
	typename std::vector<std::unique_ptr<Emitter>>::const_iterator end()   const noexcept;

	std::size_t numEmitters() const;

private:
	std::vector<std::unique_ptr<Emitter>> m_emitters;
	std::vector<std::unique_ptr<EmitterMetadata>> m_emitterMetadataBuffer;
	PrimitiveStorage m_primitiveStorage;
};

}// end namespace ph