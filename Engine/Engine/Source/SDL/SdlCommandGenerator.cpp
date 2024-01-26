#include "SDL/SdlCommandGenerator.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/Introspect/SdlOutputContext.h"
#include "SDL/ISdlResource.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/SdlOutputClauses.h"
#include "SDL/SdlInlinePacketInterface.h"
#include "SDL/SdlNamedOutputClauses.h"
#include "Utility/SemanticVersion.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Utility/string_utils.h>

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
	, m_packetInterface(std::make_unique<SdlInlinePacketInterface>())
	, m_sceneWorkingDirectory(sceneWorkingDirectory)
	, m_numGeneratedCommands(0)
	, m_numGenerationErrors(0)
	, m_stringBuffers()
{}

SdlCommandGenerator::~SdlCommandGenerator() = default;

SdlDataPacketInterface& SdlCommandGenerator::getPacketInterface()
{
	PH_ASSERT(m_packetInterface);
	return *m_packetInterface;
}

void SdlCommandGenerator::generateResourceCommand(
	const ISdlResource* const resource,
	std::string_view resourceName)
{
	if(!resource)
	{
		return;
	}

	std::string generatedCommand = borrowStringBuffer();
	const SdlClass* clazz = resource->getDynamicSdlClass();
	PH_ASSERT(clazz);

	try
	{
		// Do not start the generation if the class is not a target
		if(!hasTarget(clazz))
		{
			throw SdlSaveError("unsupported target class");
		}

		// TODO: resue output context
		SdlOutputContext ctx;
		if(!beginCommand(clazz, &ctx))
		{
			return;
		}

		// TODO: reuse clause buffer
		SdlOutputClauses clauses;
		saveResource(resource, ctx, clauses);

		generateLoadCommand(
			*resource,
			ctx,
			resourceName,
			clauses,
			generatedCommand);

		if(!generatedCommand.empty())
		{
			commandGenerated(generatedCommand, ctx);
		}

		endCommand();
	}
	catch(const SdlException& e)
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

	returnStringBuffer(std::move(generatedCommand));
}

void SdlCommandGenerator::generateCachedNamedDataPacketCommand(
	const SdlNamedOutputClauses& namedClauses)
{
	std::string generatedCommand = borrowStringBuffer();

	try
	{
		// TODO: resue output context
		SdlOutputContext ctx;
		if(!beginCommand(nullptr, &ctx))
		{
			return;
		}

		SdlNamedOutputClauses& namedClauses = *ctx.getNamedOutputClauses();
		for(std::size_t ni = 0; ni < namedClauses.numNamedOutputClauses(); ++ni)
		{
			const auto packetName = namedClauses.getName(ni);

			// Cached packet name is quoted if contains any whitespace
			const bool hasWhitespace = string_utils::has_any_of(packetName, string_utils::get_whitespaces());
			generatedCommand += hasWhitespace ? "packet $\"" : "packet $";
			generatedCommand += packetName;
			generatedCommand += hasWhitespace ? "\" = " : " = ";

			getPacketInterface().generate(
				namedClauses.getOutputClauses(ni), ctx, "", nullptr, generatedCommand);

			generatedCommand += ";\n";

			++m_numGeneratedCommands;
		}

		if(!generatedCommand.empty())
		{
			commandGenerated(generatedCommand, ctx);
		}
		
		endCommand();
	}
	catch(const SdlException& e)
	{
		PH_LOG_WARNING(SdlCommandGenerator, 
			"error generating cached name data packet command -> {}", e.whatStr());

		++m_numGenerationErrors;
	}

	returnStringBuffer(std::move(generatedCommand));
}

void SdlCommandGenerator::generateVersionCommand(const SemanticVersion& version)
{
	try
	{
		// TODO: resue output context
		SdlOutputContext ctx;
		if(!beginCommand(nullptr, &ctx))
		{
			return;
		}

		// TODO: reuse string buffer
		std::string generatedCommand = "#version ";
		generatedCommand += version.toString();
		generatedCommand += ";\n";
		commandGenerated(generatedCommand, ctx);

		endCommand();
	}
	catch(const SdlException& e)
	{
		PH_LOG_WARNING(SdlCommandGenerator,
			"error generating version command (intended version: {}) -> {}",
			version.toString(), e.whatStr());

		++m_numGenerationErrors;
	}
}

void SdlCommandGenerator::setPacketInterface(std::unique_ptr<SdlDataPacketInterface> interface)
{
	m_packetInterface = std::move(interface);
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
	const SdlOutputContext& ctx,
	std::string_view resourceName,
	const SdlOutputClauses& clauses,
	std::string& out_commandStr)
{
	const SdlClass* resourceClass = ctx.getSrcClass();
	PH_ASSERT(resource.getDynamicSdlClass() == resourceClass);

	appendFullSdlType(resourceClass, out_commandStr);

	// TODO: phantom

	// Resource name is quoted if contains any whitespace
	const bool hasWhitespace = string_utils::has_any_of(resourceName, string_utils::get_whitespaces());
	out_commandStr += hasWhitespace ? " @\"" : " @";
	out_commandStr += resourceName;
	out_commandStr += hasWhitespace ? "\" = " : " = ";

	getPacketInterface().generate(
		clauses, ctx, resourceName, &resource, out_commandStr);

	out_commandStr += ";\n";

	++m_numGeneratedCommands;
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
