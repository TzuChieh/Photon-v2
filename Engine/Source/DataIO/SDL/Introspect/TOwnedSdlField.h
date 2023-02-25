#pragma once

#include "DataIO/SDL/Introspect/SdlField.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/Introspect/SdlOutputContext.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/Introspect/EFieldImportance.h"
#include "DataIO/SDL/SdlInputPayload.h"
#include "DataIO/SDL/SdlOutputPayload.h"
#include "DataIO/SDL/SdlResourceId.h"

#include <utility>
#include <string>

namespace ph { class ISdlResource; }

namespace ph
{

/*! @brief Abstraction for a value that is owned by a SDL resource.
Governs how a field should be initialized on the policy level.
*/
template<typename Owner>
class TOwnedSdlField : public SdlField
{
public:
	TOwnedSdlField(std::string typeName, std::string valueName);

	/*! @brief Set the value of the field to a default one.
	Since the field class is templatized by `Owner` type, we are able to define default value on
	a per owner type basis.
	*/
	virtual void setValueToDefault(Owner& owner) const = 0;

	/*! @brief Convert the value of the field to human-readable string.
	*/
	virtual std::string valueToString(const Owner& owner) const = 0;

	/*! @brief Get all SDL resources associated by @p owner.
	@param owner Owner instance in question.
	@return SDL resources associated with this field by the owner. Never contains null. Appends to
	existing ones.
	*/
	virtual void ownedResources(
		const Owner& owner,
		std::vector<const ISdlResource*>& out_resources) const = 0;

	/*! @brief Acquire value and store in the owner's field.
	The loading process will follow a series of preset policy. In addition,
	failed loading attempt may be recovered if needed.
	*/
	void fromSdl(
		Owner&                 owner, 
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const;

	void toSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const;

	EFieldImportance getImportance() const;

protected:
	/*! @brief Load SDL value to actual value and store it in the owner's field.
	Implementations are highly encouraged to throw SdlLoadError if the loading
	process is not successful. This will allow things such as automatic
	fallback to work according to field policies.
	@param payload The SDL representation to be loaded into actual value.
	*/
	virtual void loadFromSdl(
		Owner&                 owner, 
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const = 0;

	/*! @brief Convert actual value back to SDL value.
	Saving a loaded value as SDL value should rarely fail--as loaded value has been
	properly handled by the loading process already. In case of failure, throw SdlSaveError 
	and provide detailed reason describing the event.
	@param out_payload The SDL representation for the actual value.
	*/
	virtual void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const = 0;

	/*! @brief Sets the importance of the field.
	Different importance affect the underlying policy used during the import
	and export of the field, e.g., whether warnings are emitted.
	*/
	TOwnedSdlField& setImportance(EFieldImportance importance);

	template<typename DstType, typename SrcType>
	DstType* castTo(SrcType* const srcInstance) const;

private:
	EFieldImportance m_importance;

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

// In-header Implementations:

template<typename Owner>
inline TOwnedSdlField<Owner>::TOwnedSdlField(std::string typeName, std::string valueName) : 

	SdlField(std::move(typeName), std::move(valueName)),

	m_importance(EFieldImportance::NiceToHave)
{}

template<typename Owner>
inline void TOwnedSdlField<Owner>::fromSdl(
	Owner&                 owner,
	const SdlInputPayload& payload,
	const SdlInputContext& ctx) const
{
	try
	{
		loadFromSdl(owner, payload, ctx);
	}
	catch(const SdlLoadError& e)
	{
		if(isFallbackEnabled())
		{
			setValueToDefault(owner);

			// Always log for recovered failed loading attempt
			// (as the user provided a SDL value string for the field, 
			// a successful parse was expected)
			//
			PH_LOG_WARNING(SdlField, "load error from {} -> {}; value defaults to {}", 
				sdl::gen_pretty_name(ctx.getSrcClass(), this),
				e.whatStr(),
				valueToString(owner));
		}
		else
		{
			// Let caller handle the error if fallback is disabled
			//
			throw SdlLoadError(
				"load error from " + sdl::gen_pretty_name(ctx.getSrcClass(), this) +
				" -> " + e.whatStr() + "; " +
				"value left uninitialized");
		}
	}
}

template<typename Owner>
inline void TOwnedSdlField<Owner>::toSdl(
	const Owner&            owner,
	SdlOutputPayload&       out_payload,
	const SdlOutputContext& ctx) const
{
	try
	{
		saveToSdl(owner, out_payload, ctx);
	}
	catch(const SdlSaveError& e)
	{
		// Provide more information and let caller handle the error
		//
		throw SdlSaveError(
			"save error from " + sdl::gen_pretty_name(ctx.getSrcClass(), this) +
			" -> " + e.whatStr());
	}
}

template<typename Owner>
inline TOwnedSdlField<Owner>& TOwnedSdlField<Owner>::setImportance(const EFieldImportance importance)
{
	m_importance = importance;

	return *this;
}

template<typename Owner>
template<typename DstType, typename SrcType>
inline DstType* TOwnedSdlField<Owner>::castTo(SrcType* const srcInstance) const
{
	try
	{
		return sdl::cast_to<DstType>(srcInstance);
	}
	catch(const SdlException& e)
	{
		throw_formatted<SdlException>(
			"input resource is not owned by this field <{}> ({})",
			genPrettyName(), e.whatStr());
	}
}

template<typename Owner>
inline EFieldImportance TOwnedSdlField<Owner>::getImportance() const
{
	return m_importance;
}

}// end namespace ph
