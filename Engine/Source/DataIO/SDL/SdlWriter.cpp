#include "DataIO/SDL/SdlWriter.h"
#include "DataIO/Stream/FormattedTextFileOutputStream.h"
#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlOutputContext.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "Common/logging.h"
#include "Common/config.h"

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlWriter, SDL);

namespace
{

void write_clause_to_stream(const SdlOutputPayload& payload, FormattedTextFileOutputStream& stream);

}

SdlWriter::SdlWriter() :
	SdlWriter("untitled-scene", Path("./"))
{}

SdlWriter::SdlWriter(std::string sceneName, Path workingDirectory) : 
	m_sceneName       (std::move(sceneName)),
	m_workingDirectory(std::move(workingDirectory)),
	m_refResolver     ()
{}

void SdlWriter::write(const SceneDescription& scene)
{
	// TODO: currently will overwrite existing file; should provide options for whether to append

	// Scene file resides in a folder with the same name as it may be accompanied with data files
	Path sceneFilePath = m_workingDirectory.append(m_sceneName);
	sceneFilePath.createDirectory();
	sceneFilePath.append(m_sceneName + ".p2");

	m_refResolver.analyze(scene);

	FormattedTextFileOutputStream sceneFile(sceneFilePath);
	sceneFile.writeString("# version {}\n", PH_PSDL_VERSION);

	OutputPayloads payloads;
	SdlOutputContext ctx(&m_refResolver, m_workingDirectory, nullptr);

	for(const ISdlResource* resource = m_refResolver.dispatch(); 
	    resource != nullptr; 
	    resource = m_refResolver.dispatch())
	{
		const SdlClass* const sdlClass = resource->getDynamicSdlClass();

		ctx.setSrcClass(sdlClass);
		payloads.clear();

		try
		{
			sdlClass->saveResource(*resource, payloads, ctx);
		}
		catch(const SdlSaveError& e)
		{
			std::string resourceName = std::string(m_refResolver.getResourceName(resource));
			if(resourceName.empty())
			{
				resourceName = "(unavailable)";
			}

			PH_LOG_WARNING(SdlWriter, "error saving resource {} -> {}, skipping this resource", 
				resourceName, e.whatStr());

			continue;
		}

		sceneFile.writeString("+> {}({}) ", sdlClass->genCategoryName(), sdlClass->getTypeName());
		for(std::size_t payloadIdx = 0; payloadIdx < payloads.numPayloads(); ++payloadIdx)
		{
			const SdlOutputPayload& payload = payloads[payloadIdx];
			write_clause_to_stream(payload, sceneFile);
		}
		sceneFile.writeNewLine();
	}
}

namespace
{

void write_clause_to_stream(const SdlOutputPayload& payload, FormattedTextFileOutputStream& stream)
{
	PH_ASSERT(stream);

	stream.writeChar('[');

	stream.writeString(payload.type);

	stream.writeChar(' ');

	stream.writeString(payload.name);
	if(payload.hasTag())
	{
		stream.writeString(": ");
		stream.writeString(payload.tag);
	}

	stream.writeChar(' ');

	stream.writeString(payload.value);

	stream.writeChar(']');
}

}

}// end namespace ph
