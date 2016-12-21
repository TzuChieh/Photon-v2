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
	void clear();
	void add(std::unique_ptr<Primitive> primitive);
	void add(std::unique_ptr<PrimitiveMetadata> metadata);

	typename std::vector<std::unique_ptr<Primitive>>::iterator       begin() noexcept;
	typename std::vector<std::unique_ptr<Primitive>>::const_iterator begin() const noexcept;
	typename std::vector<std::unique_ptr<Primitive>>::iterator       end()   noexcept;
	typename std::vector<std::unique_ptr<Primitive>>::const_iterator end()   const noexcept;

	std::size_t numPrimitives() const;

private:
	std::vector<std::unique_ptr<Primitive>> m_primitives;
	std::vector<std::unique_ptr<PrimitiveMetadata>> m_metadataBuffer;
};

}// end namespace ph