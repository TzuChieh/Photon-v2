#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"
#include "DataIO/SDL/NamedResourceStorage.h"

#include <string>
#include <memory>
#include <utility>
#include <type_traits>

namespace ph
{

class ISdlResource;

template<typename T, typename Owner>
class TSdlReference : TOwnedSdlField<Owner>
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T must be a SDL resource (derive from ISdlResource)");

public:
	TSdlReference(
		std::string                 valueName,
		std::shared_ptr<T> Owner::* valuePtr);

	void setValueToDefault(Owner& owner) override;
	std::string valueToString(const Owner& owner) const override;

private:
	void loadFromSdl(
		Owner&             owner,
		const std::string& sdlValue,
		SdlInputContext&   ctx) override;

	void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override;

	std::shared_ptr<T> Owner::* m_valuePtr;
};

// In-header Implementations:

template<typename T, typename Owner>
inline TSdlReference<T, Owner>::TSdlReference(
	std::string                       valueName,
	std::shared_ptr<T> Owner::* const valuePtr) : 

	TOwnedSdlField<Owner>("", std::move(valueName)),

	m_valuePtr(valuePtr)
{
	PH_ASSERT(m_valuePtr);
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::setValueToDefault(Owner& owner)
{
	// Nothing to set; default value for a SDL resource defined as nullptr
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::loadFromSdl(
	Owner&             owner,
	const std::string& sdlValue,
	SdlInputContext&   ctx)
{
	
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::convertToSdl(
	const Owner& owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdl);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
