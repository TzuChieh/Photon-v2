#pragma once

#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/ValueClause.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"

#include <cstddef>
#include <utility>

namespace ph
{

// TODO: should be sdl value
template<typename StructType>
class TGroupedFields : public SdlStruct
{
public:
	explicit TGroupedFields(std::string name);

	void fromSdl(
		StructType&            structObj,
		const ValueClause*     clauses,
		std::size_t            numClauses,
		const SdlInputContext& ctx) const;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;



private:
	TBasicSdlFieldSet m_fields;
};

// In-header Implementations:

template<typename Owner>
inline TGroupedFields<Owner>::TGroupedFields(std::string name) : 
	SdlStruct(std::move(name))
{}

template<typename Owner>
inline void TGroupedFields<Owner>::fromSdl(
	Owner&                 owner,
	const ValueClause*     clauses,
	std::size_t            numClauses,
	const SdlInputContext& ctx) const
{

}

template<typename Owner>
inline std::size_t TGroupedFields<Owner>::numFields() const
{
	return m_fields.numFields();
}

template<typename Owner>
inline const SdlField* TGroupedFields<Owner>::getField(const std::size_t index) const
{
	return m_fields.getField(index);
}

}// end namespace ph
