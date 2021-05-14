#pragma once

namespace ph
{

// Information required for adding the struct members to another struct

template<typename StructType, typename Owner>
class TSdlStructFieldStump
{
public:
	explicit TSdlStructFieldStump(StructType Owner::* structObjPtr);

	// TODO: funcs for optional prefix names or some override properties?

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

}// end namespace ph
