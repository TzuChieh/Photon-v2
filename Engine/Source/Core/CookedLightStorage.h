#pragma once

#include "Core/Emitter/Emitter.h"
#include "Core/Emitter/EmitterMetadata.h"

#include <vector>
#include <memory>

namespace ph
{

class CookedLightStorage final
{
public:
	void clear();
	void add(std::unique_ptr<Emitter> emitter);
	void add(std::unique_ptr<EmitterMetadata> metadata);

	typename std::vector<std::unique_ptr<Emitter>>::iterator       begin() noexcept;
	typename std::vector<std::unique_ptr<Emitter>>::const_iterator begin() const noexcept;
	typename std::vector<std::unique_ptr<Emitter>>::iterator       end()   noexcept;
	typename std::vector<std::unique_ptr<Emitter>>::const_iterator end()   const noexcept;

	std::size_t numEmitters() const;

	const Emitter& operator [] (const std::size_t index) const;

private:
	std::vector<std::unique_ptr<Emitter>> m_emitters;
	std::vector<std::unique_ptr<EmitterMetadata>> m_emitterMetadataBuffer;
};

}// end namespace ph