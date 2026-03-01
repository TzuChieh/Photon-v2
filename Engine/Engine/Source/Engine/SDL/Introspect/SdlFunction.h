#pragma once

#include "Engine/SDL/Introspect/ISdlInstantiable.h"
#include "Engine/SDL/sdl_fwd.h"
#include "Engine/SDL/Definition/SdlUserSpec.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <string>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlFunction, SDL);

class SdlFunction : public ISdlInstantiable
{
public:
	SdlFunction();

	/*! @brief Call the function.
	@param resource The target resource the function may operate on. Can use null for
	static functions (check `isStatic()`).
	@param instantiated An optional functor instance. If specified, the function will call this functor
	rather than a temporary one.
	*/
	virtual void call(
		ISdlResource*          resource,
		const SdlInstantiated* instantiated,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const = 0;

	// TODO: saveCall() & asyncCall() ?

	/*! @brief Get the number of parameters the function accepts.
	*/
	virtual std::size_t numParams() const = 0;

	/*! @brief Get a parameter of the function by index.
	@param index The index of the parameter to retrieve. Must be less than `numParams()`.
	*/
	virtual const SdlField* getParam(std::size_t index) const = 0;

	/*! @brief Whether the function is static.
	Static functions do not require a resource instance to be called.
	*/
	virtual bool isStatic() const = 0;

	SdlInstantiated instantiate() const override = 0;

	std::size_t numFields() const override final;
	const SdlField* getField(std::size_t index) const override final;
	std::string_view getTypeName() const override;
	std::string_view getDescription() const override;

	/*! @brief Call the function with a default functor instance.
	*/
	void call(
		ISdlResource*          resource,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const;

	/*!
	@return Function name.
	*/
	std::string_view getName() const;

	/*! @brief Generate a human-readable name for the function.
	*/
	std::string genPrettyName() const;

	/*! @brief Get the user specifications for the function.
	*/
	const SdlUserSpec& getUserSpec() const;

protected:
	/*! @brief Set the name of the function.
	@return `*this` for chaining.
	*/
	SdlFunction& setName(std::string name);

	/*! @brief Set the description of the function.
	@return `*this` for chaining.
	*/
	SdlFunction& setDescription(std::string description);

	/*! @brief Set the user specifications for the function.
	@return `*this` for chaining.
	*/
	SdlFunction& setUserSpec(SdlUserSpec userSpec);

private:
	std::string m_name;
	std::string m_description;
	SdlUserSpec m_userSpec;
};

// In-header Implementations:

inline void SdlFunction::call(
	ISdlResource*          resource,
	SdlInputClauses&       clauses,
	const SdlInputContext& ctx) const
{
	call(resource, nullptr, clauses, ctx);
}

inline std::size_t SdlFunction::numFields() const
{
	return numParams();
}

inline const SdlField* SdlFunction::getField(std::size_t index) const
{
	return getParam(index);
}

inline std::string_view SdlFunction::getTypeName() const
{
	return getName();
}

inline std::string_view SdlFunction::getName() const
{
	PH_ASSERT(!m_name.empty());
	return m_name;
}

inline std::string_view SdlFunction::getDescription() const
{
	return m_description;
}

inline const SdlUserSpec& SdlFunction::getUserSpec() const
{
	return m_userSpec;
}

}// end namespace ph
