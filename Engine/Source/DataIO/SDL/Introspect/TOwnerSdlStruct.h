#pragma once

#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/ValueClauses.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"

#include <cstddef>

namespace ph
{

class SdlInputContext;

template<typename StructType, typename FieldSet = TBasicSdlFieldSet<TOwnedSdlField<StructType>>>
class TOwnerSdlStruct : public SdlStruct
{
public:
	explicit TOwnerSdlStruct(std::string name);

	void fromSdl(
		StructType&            structObj,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;

	template<typename T>
	TOwnerSdlStruct& addField(T field);

	const FieldSet& getFields() const;

private:
	FieldSet m_fields;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TOwnerSdlStruct.ipp"
