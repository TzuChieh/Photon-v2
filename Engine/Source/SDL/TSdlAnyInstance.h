#pragma once

#include "SDL/sdl_fwd.h"

#include <variant>
#include <type_traits>

namespace ph
{

/*! @brief References a SDL object.
This is a lightweight utility for referencing SDL objects. Following objects are valid instances
to be referenced by this type:
- Objects of SDL classes
- Objects of SDL structs
*/
template<bool IS_CONST>
class TSdlAnyInstance
{
public:
	TSdlAnyInstance();

	template<typename T>
	TSdlAnyInstance(T* instance);

	template<typename T>
	auto* get() const;

	template<typename T>
	const auto* getConst() const;

	const SdlClass* getClass() const;
	const SdlStruct* getStruct() const;

	operator bool() const;

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
		const SdlStruct*>;

	InstanceType m_instance;
	MetaType m_meta;
};

}// end namespace ph

#include "SDL/TSdlAnyInstance.ipp"
