#pragma once

#include "DataIO/SDL/Introspect/SdlFunction.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/ValueClause.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"

#include <cstddef>
#include <utility>

namespace ph
{

template<typename MethodStruct, typename TargetType>
class TSdlMethod : public SdlFunction
{
public:
	explicit TSdlMethod(std::string name);

	void callMethod(
		TargetType&            targetType,
		const ValueClause*     clauses,
		std::size_t            numClauses,
		const SdlInputContext& ctx) const;

	void loadParameters(
		MethodStruct&          parameterStruct,
		const ValueClause*     clauses,
		std::size_t            numClauses,
		const SdlInputContext& ctx) const;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;

private:
	TBasicSdlFieldSet<TOwnedSdlField<MethodStruct>> m_fields;
};

// In-header Implementations:

template<typename MethodStruct, typename TargetType>
inline TSdlMethod<MethodStruct, TargetType>::TSdlMethod(std::string name) :
	SdlFunction(std::move(name))
{}

template<typename MethodStruct, typename TargetType>
inline void TSdlMethod<MethodStruct, TargetType>::callMethod(
	TargetType&              targetType,
	const ValueClause* const clauses,
	const std::size_t        numClauses,
	const SdlInputContext&   ctx) const
{

}

template<typename MethodStruct, typename TargetType>
inline void TSdlMethod<MethodStruct, TargetType>::loadParameters(
	MethodStruct&            parameterStruct,
	const ValueClause* const clauses,
	const std::size_t        numClauses,
	const SdlInputContext&   ctx) const
{

}

template<typename MethodStruct, typename TargetType>
inline std::size_t TSdlMethod<MethodStruct, TargetType>::numFields() const
{
	return m_fields.numFields();
}

template<typename MethodStruct, typename TargetType>
inline const SdlField* TSdlMethod<MethodStruct, TargetType>::getField(const std::size_t index) const
{
	return m_fields.getField(index);
}

}// end namespace ph
