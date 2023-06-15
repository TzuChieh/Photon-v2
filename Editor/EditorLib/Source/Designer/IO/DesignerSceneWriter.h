#pragma once

#include "Designer/IO/DesignerDataPacketInterface.h"

#include <SDL/SdlCommandGenerator.h>
#include <SDL/SdlDependencyResolver.h>

namespace ph { class FormattedTextOutputStream; }
namespace ph { class Path; }

namespace ph::editor
{

class DesignerScene;

/*! @brief Saving utility for designer scene.
Writes .pds (Photon Designer Scene) file.
*/
class DesignerSceneWriter : public SdlCommandGenerator
{
public:
	DesignerSceneWriter();
	explicit DesignerSceneWriter(const Path& sceneWorkingDirectory);
	~DesignerSceneWriter() override;

	SdlDataPacketInterface& getPacketInterface() override;

	/*! @brief Writes the scene to the writer-specified destination.
	*/
	void write(const DesignerScene& scene);

protected:
	bool beginCommand(const SdlClass* targetClass) override;

	void saveResource(
		const ISdlResource* resource,
		const SdlClass* resourceClass,
		SdlOutputClauses& clauses) override;

	void commandGenerated(std::string_view commandStr) override;
	void endCommand() override;

private:
	void saveSceneToFile(const DesignerScene& scene, const Path& filePath);

	SdlDependencyResolver m_resolver;
	DesignerDataPacketInterface m_packetInterface;
	FormattedTextOutputStream* m_fileStream;
};

}// end namespace ph::editor
