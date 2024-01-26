#pragma once

#include "SDL/Introspect/ISdlInstantiable.h"
#include "SDL/sdl_fwd.h"
#include "Utility/TAnyPtr.h"

#include <Common/logging.h>

#include <cstddef>
#include <string>
#include <vector>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlStruct, SDL);

/*!
SDL struct is designed to be a simple grouped data carrier without the burden of inheritance and 
member functions for efficiency and ease of reuse.

`TSdl` provides a simplified interface for initializing instances to default values.
*/
class SdlStruct : public ISdlInstantiable
{
public:
	explicit SdlStruct(std::string typeName);

	/*! @brief Initialize a struct object from value clauses.
	How the object will be initialized depends on the struct's SDL definition.
	*/
	virtual void initObject(
		AnyNonConstPtr         obj,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const = 0;

	/*! @brief Initialize a struct object to default values.
	Default values are defined by the struct's SDL definition.
	*/
	virtual void initDefaultObject(AnyNonConstPtr obj) const = 0;

	virtual void saveObject(
		AnyConstPtr             obj,
		SdlOutputClauses&       clauses,
		const SdlOutputContext& ctx) const = 0;

	/*! @brief Get all SDL resources referenced by @p obj.
	@param obj The object that may contain SDL resources.
	@param out_resources SDL resources referenced by @p obj. Never contains null.
	Appends to existing ones.
	*/
	virtual void referencedResources(
		AnyConstPtr obj,
		std::vector<const ISdlResource*>& out_resources) const = 0;

	std::size_t numFields() const override = 0;
	const SdlField* getField(std::size_t index) const override = 0;

	std::string_view getTypeName() const override;
	std::string_view getDescription() const override;

protected:
	SdlStruct& setDescription(std::string description);

private:
	std::string m_typeName;
	std::string m_description;
};

// In-header Implementations:

inline std::string_view SdlStruct::getTypeName() const
{
	return m_typeName;
}

inline std::string_view SdlStruct::getDescription() const
{
	return m_description;
}

}// end namespace ph
