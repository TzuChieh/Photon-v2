#pragma once

#include "SDL/Introspect/SdlField.h"
#include "SDL/sdl_fwd.h"
#include "SDL/Introspect/SdlNativeData.h"

namespace ph
{

/*! @brief Abstraction for a value that is owned by some owner type.
Governs how a field should be initialized on the policy level.
@tparam Owner The owner type. May be any C++ class/struct type including SDL resource types.
*/
template<typename Owner>
class TSdlOwnedField : public SdlField
{
public:
	TSdlOwnedField(
		std::string typeName, 
		std::string valueName);

	TSdlOwnedField(
		std::string typeName, 
		std::string valueName,
		std::string typeSignature);

	/*! @brief Set the value of the field to a default one.
	Since the field class is templatized by `Owner` type, we are able to define default value on
	a per owner type basis.
	*/
	virtual void ownedValueToDefault(Owner& owner) const = 0;

	/*! @brief Convert the value of the field to human-readable string.
	*/
	virtual std::string valueToString(const Owner& owner) const = 0;

	/*! @brief Get all SDL resources associated by @p owner.
	@param owner Owner instance in question.
	@param out_resources SDL resources associated with this field by the owner. Never contains null. Appends to
	existing ones.
	*/
	virtual void ownedResources(
		const Owner& owner,
		std::vector<const ISdlResource*>& out_resources) const = 0;

	/*! @brief Direct access to the field memory of an owner.
	Short-lived owner objects such as function parameter structs must be kept alive during during
	this call and any further usages of the returned native data.
	*/
	virtual SdlNativeData ownedNativeData(Owner& owner) const = 0;

	SdlNativeData nativeData(SdlNonConstInstance instance) const override;

	/*! @brief Acquire value and store in the owner's field.
	The loading process will follow a series of preset policy. In addition,
	failed loading attempt may be recovered if needed.
	*/
	void fromSdl(
		Owner&                 owner, 
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const;

	void toSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const;

protected:
	/*! @brief Load SDL value to actual value and store it in the owner's field.
	Implementations are highly encouraged to throw SdlLoadError if the loading
	process is not successful. This will allow things such as automatic
	fallback to work according to field policies.
	@param owner Instance that owns the field.
	@param clause The SDL representation to be loaded into actual value.
	@param ctx The context for loading.
	*/
	virtual void loadFromSdl(
		Owner&                 owner, 
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const = 0;

	/*! @brief Convert actual value back to SDL value.
	Saving a loaded value as SDL value should rarely fail--as loaded value has been
	properly handled by the loading process already. In case of failure, throw SdlSaveError 
	and provide detailed reason describing the event.
	@param owner Instance that owns the field.
	@param out_clause The SDL representation for the actual value.
	@param ctx The context for saving.
	*/
	virtual void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const = 0;

private:
	// Nested field need the ability to wrap calls including non-public ones
	// Note that friend with partial specializations is not supported
	// in C++17. We resort to making more friend classes than needed.
	// 
	// The following friend statement is not yet supported:
	//
	// template<typename InnerType>
	// friend class TSdlNestedField<Owner, InnerType>;
	//
	// See https://stackoverflow.com/questions/44213761/partial-template-specialization-for-friend-classes
	//
	template<typename OuterType, typename InnerType>
	friend class TSdlNestedField;
};

}// end namespace ph

#include "SDL/Introspect/TSdlOwnedField.ipp"
