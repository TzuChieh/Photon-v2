#include "SDL/SdlCommandGenerator.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/ISdlResource.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/SdlOutputClauses.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Utility/SemanticVersion.h"

#include <cstddef>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlCommandGenerator, SDL);

SdlCommandGenerator::SdlCommandGenerator(TSpanView<const SdlClass*> targetClasses)
	: SdlCommandGenerator(targetClasses, Path("./temp_sdl/"))
{}

SdlCommandGenerator::SdlCommandGenerator(
	TSpanView<const SdlClass*> targetClasses, 
	const Path& sceneWorkingDirectory)

	: m_targetClasses(targetClasses.begin(), targetClasses.end())
	, m_sceneWorkingDirectory(sceneWorkingDirectory)
	, m_inlinePacketInterface(sceneWorkingDirectory)
	, m_numGeneratedCommands(0)
	, m_numGenerationErrors(0)
{}

SdlCommandGenerator::~SdlCommandGenerator() = default;

SdlDataPacketInterface& SdlCommandGenerator::getPacketInterface()
{
	return m_inlinePacketInterface;
}

void SdlCommandGenerator::generateLoadCommand(
	const ISdlResource* const resource,
	std::string_view resourceName)
{
	if(!resource)
	{
		return;
	}

	const SdlClass* clazz = resource->getDynamicSdlClass();
	PH_ASSERT(clazz);

	try
	{
		// Do not start the generation if the class is not a target
		if(!hasTarget(clazz))
		{
			throw SdlSaveError("unsupported target class");
		}

		if(!beginCommand(clazz))
		{
			return;
		}

		// TODO: reuse clause buffer
		SdlOutputClauses clauses;
		saveResource(resource, clazz, clauses);

		// TODO: reuse string buffer
		std::string generatedCommand;
		generateLoadCommand(
			*resource, 
			clazz,
			resourceName,
			clauses,
			generatedCommand);

		if(!generatedCommand.empty())
		{
			commandGenerated(generatedCommand);
			++m_numGeneratedCommands;
		}
		
		endCommand();
	}
	catch(const SdlSaveError& e)
	{
		std::string resourceNameInfo(resourceName);
		if(resourceNameInfo.empty())
		{
			resourceNameInfo = "(unavailable)";
		}

		PH_LOG_WARNING(SdlCommandGenerator, 
			"error generating load command for resource {} ({}) -> {}",
			resourceNameInfo, clazz->genPrettyName(), e.whatStr());

		++m_numGenerationErrors;
	}
}

void SdlCommandGenerator::generateVersionCommand(const SemanticVersion& version)
{
	try
	{
		if(!beginCommand(nullptr))
		{
			return;
		}

		// TODO: reuse string buffer
		std::string generatedCommand = "#version ";
		generatedCommand += version.toString();
		generatedCommand += ";\n";
		commandGenerated(generatedCommand);

		endCommand();
	}
	catch(const SdlSaveError& e)
	{
		PH_LOG_WARNING(SdlCommandGenerator,
			"error generating version command (intended version: {}) -> {}",
			version.toString(), e.whatStr());

		++m_numGenerationErrors;
	}
}

void SdlCommandGenerator::setSceneWorkingDirectory(const Path& directory)
{
	m_sceneWorkingDirectory = directory;
}

void SdlCommandGenerator::clearStats()
{
	m_numGeneratedCommands = 0;
	m_numGenerationErrors = 0;
}

bool SdlCommandGenerator::hasTarget(const SdlClass* const clazz) const
{
	const auto& result = m_targetClasses.find(clazz);
	return result != m_targetClasses.end();
}

void SdlCommandGenerator::generateLoadCommand(
	const ISdlResource& resource,
	const SdlClass* const resourceClass,
	std::string_view resourceName,
	const SdlOutputClauses& clauses,
	std::string& out_commandStr)
{
	PH_ASSERT(resource.getDynamicSdlClass() == resourceClass);

	appendFullSdlType(resourceClass, out_commandStr);
	out_commandStr += ' ';
	out_commandStr += resourceName;
	out_commandStr += " = ";

	getPacketInterface().generate(clauses, resourceClass, resourceName, &resource, out_commandStr);

	out_commandStr += ";\n";
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

}// end namespace ph
