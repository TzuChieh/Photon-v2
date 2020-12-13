#pragma once

#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"

#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace ph
{

template<typename Owner>
class TSdlClass : public SdlClass
{
public:
	explicit TSdlClass(std::string className);

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;

	const TOwnedSdlField<Owner>* getOwnedField(std::size_t index) const;

	TSdlClass& addField(std::unique_ptr<TOwnedSdlField<Owner>> field);

private:
	std::vector<std::unique_ptr<TOwnedSdlField<Owner>>> m_fields;
};

// In-header Implementations:

template<typename Owner>
inline TSdlClass<Owner>::TSdlClass(std::string className) : 
	SdlClass(std::move(className))
{}

template<typename Owner>
inline std::size_t TSdlClass<Owner>::numFields() const
{
	return m_fields.size();
}

template<typename Owner>
inline const SdlField* TSdlClass<Owner>::getField(const std::size_t index) const
{
	return getOwnedField(index);
}

template<typename Owner>
inline const TOwnedSdlField<Owner>* TSdlClass<Owner>::getOwnedField(const std::size_t index) const
{
	return index < m_fields.size() ? m_fields[index].get() : nullptr;
}

template<typename Owner>
inline TSdlClass<Owner>& TSdlClass<Owner>::addField(std::unique_ptr<TOwnedSdlField<Owner>> field)
{
	PH_ASSERT(field);

	m_fields.push_back(std::move(field));

	return *this;
}

}// end namespace ph
