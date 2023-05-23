#include "SDL/SdlCommandGenerator.h"
#include "SDL/SdlDependencyResolver.h"
#include "SDL/SceneDescription.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/ISdlResource.h"
#include "SDL/Introspect/SdlOutputContext.h"
#include "SDL/sdl_exceptions.h"
#include "Common/assertion.h"
#include "Common/logging.h"

#include <cstddef>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlCommandGenerator, SDL);

SdlCommandGenerator::SdlCommandGenerator()
	: SdlCommandGenerator(Path("./"))
{}

SdlCommandGenerator::SdlCommandGenerator(Path sceneWorkingDirectory)
	: m_sceneWorkingDirectory(std::move(sceneWorkingDirectory))
{}

SdlCommandGenerator::~SdlCommandGenerator() = default;

void SdlCommandGenerator::generateScene(const SceneDescription& scene)
{
	PH_LOG(SdlCommandGenerator, "start generating commands");

	SdlDependencyResolver resolver;
	resolver.analyze(scene);

	std::size_t numGeneratedCommands = 0;
	std::size_t numErrors = 0;
	OutputBuffer result;

	for(const ISdlResource* resource = resolver.next(); 
	    resource != nullptr; 
	    resource = resolver.next())
	{
		const SdlClass* clazz = resource->getDynamicSdlClass();
		PH_ASSERT(clazz);

		if(!beginCommand(clazz))
		{
			continue;
		}

		result.clear();

		try
		{
			saveResource(
				resource, 
				clazz,
				result.clauses,
				&resolver);

			generateLoadCommand(
				*resource, 
				clazz,
				std::string(resolver.getResourceName(resource)),
				result.clauses,
				result.commandStr);

			commandGenerated(result.commandStr);

			++numGeneratedCommands;
		}
		catch(const SdlSaveError& e)
		{
			std::string resourceName = std::string(resolver.getResourceName(resource));
			if(resourceName.empty())
			{
				resourceName = "(unavailable)";
			}

			PH_LOG_WARNING(SdlCommandGenerator, 
				"error generating command for resource {} ({}) -> {}, skipping this resource",
				resourceName, clazz->genPrettyName(), e.whatStr());

			++numErrors;
		}

		endCommand();
	}

	PH_LOG(SdlCommandGenerator, 
		"generated {} commands (errors: {})", 
		numGeneratedCommands, numErrors);
}

void SdlCommandGenerator::setSceneWorkingDirectory(Path directory)
{
	m_sceneWorkingDirectory = std::move(directory);
}

void SdlCommandGenerator::generateLoadCommand(
	const ISdlResource& resource,
	const SdlClass* const resourceClass,
	const std::string& resourceName,
	const SdlOutputClauses& clauses,
	std::string& out_commandStr)
{
	PH_ASSERT(resource.getDynamicSdlClass() == resourceClass);

	appendFullSdlType(resourceClass, out_commandStr);
	out_commandStr += ' ';
	out_commandStr += resourceName;
	out_commandStr += " = ";

	for(std::size_t clauseIdx = 0; clauseIdx < clauses.numClauses(); ++clauseIdx)
	{
		appendClause(clauses[clauseIdx], out_commandStr);
	}

	out_commandStr += '\n';
}

void SdlCommandGenerator::appendFullSdlType(
	const SdlClass* clazz,
	std::string& out_commandStr)
{
	PH_ASSERT(clazz);

	out_commandStr += sdl::category_to_string(clazz->getCategory());
	out_commandStr += '(';
	out_commandStr += clazz->getTypeName();
	out_commandStr += ')';
}

void SdlCommandGenerator::appendClause(
	const SdlOutputClause& clause,
	std::string& out_commandStr)
{
	out_commandStr += '[';
	out_commandStr += clause.type;
	out_commandStr += ' ';
	out_commandStr += clause.name;

	if(clause.hasTag())
	{
		out_commandStr += ": ";
		out_commandStr += clause.tag;
	}

	out_commandStr += ' ';
	out_commandStr += clause.value;
	out_commandStr += ']';
}

}// end namespace ph
