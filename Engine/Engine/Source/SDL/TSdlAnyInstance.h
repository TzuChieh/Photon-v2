#pragma once

#include "SDL/sdl_fwd.h"

#include <variant>
#include <type_traits>

namespace ph
{

/*! @brief References a SDL object.
This is a lightweight utility for referencing SDL objects. Following objects are valid targets
to be referenced by this type:
- Objects of SDL classes
- Objects of SDL structs
- Objects of SDL function parameter structs
*/
template<bool IS_CONST>
class TSdlAnyInstance
{
public:
	TSdlAnyInstance();

	TSdlAnyInstance(std::nullptr_t target);

	/*! @brief Reference to a SDL object.
	Allows only valid SDL objects.
	*/
	template<typename T>
	TSdlAnyInstance(T* target);

	template<typename T>
	auto* get() const;

	template<typename T>
	const auto* getConst() const;

	const SdlClass* getClass() const;
	const SdlStruct* getStruct() const;
	const SdlFunction* getFunction() const;

	bool isClass() const;
	bool isStruct() const;
	bool isFunction() const;

	const ISdlInstantiable* getInstantiable() const;

	operator bool () const;

private:
	using ClassInstanceType = std::conditional_t<IS_CONST, const ISdlResource, ISdlResource>;
	using StructInstanceType = std::conditional_t<IS_CONST, const void, void>;

	using InstanceType = std::variant<
		std::monostate, 
		ClassInstanceType*, 
		StructInstanceType*>;

	using MetaType = std::variant<
		std::monostate,
		const SdlClass*,
		const SdlStruct*,
		const SdlFunction*>;

	InstanceType m_instance;
	MetaType m_meta;
};

}// end namespace ph

#include "SDL/TSdlAnyInstance.ipp"
