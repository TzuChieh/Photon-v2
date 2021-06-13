#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"

#include <type_traits>
#include <string>
#include <exception>
#include <vector>

namespace ph
{

/*! @brief A field that lives within an inner object.
*/
// TODO: policy for changing field attributes on nested field--
// corresponding inner field attributes will not change, what to do?
template<typename OuterType, typename InnerType>
class TSdlNestedField : public TOwnedSdlField<OuterType>
{
public:
	TSdlNestedField(
		InnerType OuterType::*           innerObjPtr, 
		const TOwnedSdlField<InnerType>* innerObjField);

	void setValueToDefault(OuterType& outerObj) const override;
	std::string valueToString(const OuterType& outerObj) const override;

protected:
	void loadFromSdl(
		OuterType&             outerObj,
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const override;

	void convertToSdl(
		const OuterType& outerObj,
		std::string*     out_sdlValue,
		std::string&     out_converterMessage) const override;

private:
	InnerType OuterType::*           m_innerObjPtr;
	const TOwnedSdlField<InnerType>* m_innerObjField;
};

// In-header Implementations:

template<typename OuterType, typename InnerType>
inline TSdlNestedField<OuterType, InnerType>::TSdlNestedField(
	InnerType OuterType::* const           innerObjPtr,
	const TOwnedSdlField<InnerType>* const innerObjField) :

	TOwnedSdlField<OuterType>(
		innerObjField ? innerObjField->getTypeName()  : "unavailable",
		innerObjField ? innerObjField->getFieldName() : "unavailable"),

	m_innerObjPtr  (innerObjPtr),
	m_innerObjField(innerObjField)
{
	PH_ASSERT(m_innerObjPtr);
	PH_ASSERT(m_innerObjField);

	PH_ASSERT_MSG(static_cast<const SdlField*>(m_innerObjField) != this, 
		"setting self as inner field is forbidden (will result in infinite recursive calls)");

	setDescription(m_innerObjField->getDescription());
	enableFallback(m_innerObjField->isFallbackEnabled());
	setImportance(m_innerObjField->getImportance());
}

template<typename OuterType, typename InnerType>
inline void TSdlNestedField<OuterType, InnerType>::setValueToDefault(OuterType& outerObj) const
{
	m_innerObjField->setValueToDefault(outerObj.*m_innerObjPtr);
}

template<typename OuterType, typename InnerType>
inline std::string TSdlNestedField<OuterType, InnerType>::valueToString(const OuterType& outerObj) const
{
	return m_innerObjField->valueToString(outerObj.*m_innerObjPtr);
}

template<typename OuterType, typename InnerType>
inline void TSdlNestedField<OuterType, InnerType>::loadFromSdl(
	OuterType&             outerObj,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	m_innerObjField->loadFromSdl(
		outerObj.*m_innerObjPtr,
		sdlValue,
		ctx);
}

template<typename OuterType, typename InnerType>
void TSdlNestedField<OuterType, InnerType>::convertToSdl(
	const OuterType&   outerObj,
	std::string* const out_sdlValue,
	std::string&       out_converterMessage) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
