#include "Engine/SDL/Introspect/SdlFunction.h"
#include "Engine/SDL/sdl_helpers.h"

#include <utility>

namespace ph
{

SdlFunction::SdlFunction()
	: m_name()
	, m_snakeCaseName()
	, m_description()
	, m_userSpec()
{}

std::string SdlFunction::genPrettyName() const
{
	return sdl::gen_pretty_name(this);
}

SdlFunction& SdlFunction::setName(std::string name)
{
	m_name = std::move(name);
	m_snakeCaseName = sdl::name_to_snake_case(m_name);
	return *this;
}

SdlFunction& SdlFunction::setDescription(std::string description)
{
	m_description = std::move(description);
	return *this;
}

SdlFunction& SdlFunction::setUserSpec(SdlUserSpec userSpec)
{
	m_userSpec = std::move(userSpec);
	return *this;
}

}// end namespace ph
