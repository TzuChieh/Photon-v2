#pragma once

#include "Core/Integrator/Attribute/TAttributeHandlerInterface.h"
#include "Common/assertion.h"
#include "Core/Filmic/TSamplingFilm.h"

#include <memory>

namespace ph
{

template<typename Value>
class TSamplingFilmAttributeHandler final : public TAttributeHandlerInterface<Value>
{
public:
	TSamplingFilmAttributeHandler(std::unique_ptr<TSamplingFilm<Value>> film);

	void handleSinglePut(const Value& value) override;

private:
	std::unique_ptr<TSamplingFilm<Value>> m_film;
};

// Implementations:

template<typename Value>
inline TSamplingFilmAttributeHandler<Value>::TSamplingFilmAttributeHandler(
	std::unique_ptr<TSamplingFilm<Value>> film) : 
	m_film(std::move(film))
{
	PH_ASSERT(m_film);
}

template<typename Value>
inline void TSamplingFilmAttributeHandler<Value>::handleSinglePut(const Value& value)
{
	m_film->addSample(value);
}

}// end namespace ph