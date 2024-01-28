#include "SDL/SdlNamedOutputClauses.h"
#include "SDL/sdl_exceptions.h"

#include <Common/logging.h>

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlNamedOutputClauses, SDL);

void SdlNamedOutputClauses::addUnique(
	SdlOutputClauses clauses,
	std::string_view clausesName)
{
	if(clausesName.empty())
	{
		throw SdlLoadError(
			"cannot add named SDL output clauses due to empty name");
	}

	const auto& iter = m_clausesNames.find(std::string(clausesName));
	if(iter != m_clausesNames.end())
	{
		PH_LOG(SdlNamedOutputClauses, Warning,
			"duplicated named SDL output clauses detected, overwriting (name: {})", clausesName);
	}

	add(std::move(clauses), clausesName);
}

void SdlNamedOutputClauses::addOrUpdate(
	SdlOutputClauses clauses,
	std::string_view clausesName)
{
	add(std::move(clauses), clausesName);
}

std::string_view SdlNamedOutputClauses::addOrUpdate(SdlOutputClauses packet)
{
	const auto packetIndex = m_namedClauses.size();
	addOrUpdate(std::move(packet), "__" + std::to_string(packetIndex));
	return m_namedClauses.back().name;
}

void SdlNamedOutputClauses::add(
	SdlOutputClauses clauses,
	std::string_view clausesName)
{
	if(clausesName.empty())
	{
		throw SdlLoadError(
			"cannot add named SDL output clauses due to empty name");
	}

	m_namedClauses.push_back({
		.name = std::string(clausesName), 
		.clauses = std::move(clauses)});

	m_clausesNames.insert(std::string(clausesName));
}

}// end namespace ph
