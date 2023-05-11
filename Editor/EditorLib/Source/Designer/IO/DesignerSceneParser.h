#pragma once

#include <SDL/SdlDataPacketParser.h>
#include <DataIO/FileSystem/Path.h>

namespace ph::editor
{

class DesignerScene;

class DesignerSceneParser : public SdlDataPacketParser
{
public:
	explicit DesignerSceneParser(TSpanView<const SdlClass*> targetClasses);

	/*bool beingPacket(const SdlClass* targetClass) override;
	ISdlResource* createResource(const SdlClass* targetClass) override;
	void parsedResource(ISdlResource* resource) override;
	bool endPacket() override;*/

	void parseScene(const Path& file, DesignerScene* out_scene);
};

}// end namespace ph::editor
