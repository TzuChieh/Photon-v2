#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/SdlIOUtils.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"

#include <type_traits>
#include <string>
#include <exception>
#include <vector>

namespace ph
{

template<typename OuterType, typename InnerType>
class TSdlNestedField : public TOwnedSdlField<OuterType>
{
public:
	TSdlNestedField(
		InnerType OuterType::*           innerObjPtr, 
		const TOwnedSdlField<InnerType>& innerField);

	std::string valueToString(const OuterType& outerObj) const override;

private:
	void loadFromSdl(
		OuterType&             outerObj,
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const override;

	void convertToSdl(
		const OuterType& outerObj,
		std::string*     out_sdlValue,
		std::string&     out_converterMessage) const override;

	InnerType OuterType::*           m_innerObjPtr;
	const TOwnedSdlField<InnerType>& m_innerField;
};

// In-header Implementations:

template<typename OuterType, typename InnerType>
inline TSdlNestedField<OuterType, InnerType>::TSdlNestedField(
	InnerType OuterType::* const     innerObjPtr,
	const TOwnedSdlField<InnerType>& innerField) :

	TOwnedSdlField<OuterType>(
		innerField.getTypeName(),
		innerField.getFieldName()),

	m_innerObjPtr(innerObjPtr),
	m_innerField (innerField)
{
	PH_ASSERT(m_innerObjPtr);


}

template<typename OuterType, typename InnerType>
inline std::string TSdlNestedField<OuterType, InnerType>::valueToString(const OuterType& outerObj) const
{
	return "[" + std::to_string(getValue(owner).size()) + " vector3 values...]";
}

template<typename OuterType, typename InnerType>
inline void TSdlNestedField<OuterType, InnerType>::loadFromSdl(
	OuterType&             outerObj,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	setValue(owner, SdlIOUtils::loadVector3RArray(sdlValue));
}

template<typename OuterType, typename InnerType>
void TSdlNestedField<OuterType, InnerType>::convertToSdl(
	const OuterType&   outerObj,
	std::string* const out_sdlValue,
	std::string&       out_converterMessage) const
{
	m_innerField.convertToSdl(
		owner
		)

	PH_ASSERT(out_sdlValue);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
