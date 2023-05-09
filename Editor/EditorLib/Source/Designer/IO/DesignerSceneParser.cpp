#include "Designer/IO/DesignerSceneParser.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>

namespace ph::editor
{

DesignerSceneParser::DesignerSceneParser(TSpanView<const SdlClass*> targetClasses)
	: SdlDataPacketParser(targetClasses)
{}

bool DesignerSceneParser::beingPacket(const SdlClass* targetClass)
{
	// TODO
	return false;
}

ISdlResource* DesignerSceneParser::createResource(const SdlClass* targetClass)
{
	// TODO
	return nullptr;
}

void DesignerSceneParser::parsedResource(ISdlResource* resource)
{
	// TODO
}

bool DesignerSceneParser::endPacket()
{
	// TODO
	return false;
}

void DesignerSceneParser::parseScene(const Path& file, DesignerScene* const out_scene)
{
	PH_ASSERT(out_scene);

	// TODO
}

}// end namespace ph::editor
