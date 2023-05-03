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

Field properties (importance, fall back, etc.) are initialized to the same
values as inner field, and can be overridden by manually setting the properties
later.
*/
template<typename OuterType, typename InnerType>
class TSdlNestedField : public TOwnedSdlField<OuterType>
{
public:
	TSdlNestedField(
		InnerType OuterType::*           innerObjPtr, 
		const TOwnedSdlField<InnerType>* innerObjField);

	void setValueToDefault(OuterType& outerObj) const override;
	std::string valueToString(const OuterType& outerObj) const override;

	void ownedResources(
		const OuterType& outerObj,
		std::vector<const ISdlResource*>& out_resources) const override;

	SdlNativeData ownedNativeData(OuterType& outerObj) const override;

protected:
	void loadFromSdl(
		OuterType&             outerObj,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override;

	void saveToSdl(
		const OuterType&        outerObj,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override;

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

	this->setDescription(m_innerObjField->getDescription());
	this->setEnableFallback(m_innerObjField->isFallbackEnabled());
	this->setImportance(m_innerObjField->getImportance());
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
inline void TSdlNestedField<OuterType, InnerType>::ownedResources(
	const OuterType& outerObj,
	std::vector<const ISdlResource*>& out_resources) const
{
	m_innerObjField->ownedResources(outerObj.*m_innerObjPtr, out_resources);
}

template<typename OuterType, typename InnerType>
inline SdlNativeData TSdlNestedField<OuterType, InnerType>::ownedNativeData(OuterType& outerObj) const
{
	return m_innerObjField->ownedNativeData(outerObj.*m_innerObjPtr);
}

template<typename OuterType, typename InnerType>
inline void TSdlNestedField<OuterType, InnerType>::loadFromSdl(
	OuterType&             outerObj,
	const SdlInputPayload& payload,
	const SdlInputContext& ctx) const
{
	m_innerObjField->loadFromSdl(
		outerObj.*m_innerObjPtr,
		payload,
		ctx);
}

template<typename OuterType, typename InnerType>
void TSdlNestedField<OuterType, InnerType>::saveToSdl(
	const OuterType&        outerObj,
	SdlOutputPayload&       out_payload,
	const SdlOutputContext& ctx) const
{
	m_innerObjField->saveToSdl(
		outerObj.*m_innerObjPtr,
		out_payload,
		ctx);
}

}// end namespace ph
