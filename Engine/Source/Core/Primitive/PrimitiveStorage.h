#pragma once

#include "Core/Primitive/Primitive.h"
#include "Core/Primitive/PrimitiveMetadata.h"

#include <vector>
#include <memory>

namespace ph
{

class PrimitiveStorage final
{
public:
	PrimitiveStorage() = default;

	void clear();
	void add(std::unique_ptr<Primitive> primitive);
	void add(std::vector<std::unique_ptr<Primitive>>&& primitives);
	void add(std::unique_ptr<PrimitiveMetadata> metadata);
	void add(PrimitiveStorage&& other);

	typename std::vector<std::unique_ptr<Primitive>>::iterator       begin() noexcept;
	typename std::vector<std::unique_ptr<Primitive>>::const_iterator begin() const noexcept;
	typename std::vector<std::unique_ptr<Primitive>>::iterator       end()   noexcept;
	typename std::vector<std::unique_ptr<Primitive>>::const_iterator end()   const noexcept;

	std::size_t numPrimitives() const;

	std::vector<std::unique_ptr<PrimitiveMetadata>>& getMetadataBuffer();

	// forbid copying
	PrimitiveStorage(const PrimitiveStorage& other) = delete;
	PrimitiveStorage& operator = (const PrimitiveStorage& rhs) = delete;

private:
	std::vector<std::unique_ptr<Primitive>> m_primitives;
	std::vector<std::unique_ptr<PrimitiveMetadata>> m_metadataBuffer;
};

}// end namespace ph