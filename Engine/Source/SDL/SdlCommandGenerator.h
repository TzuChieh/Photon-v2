#pragma once

#include "SDL/sdl_fwd.h"
#include "DataIO/FileSystem/Path.h"
#include "SDL/SdlInlinePacketInterface.h"
#include "Utility/TSpan.h"

#include <string_view>
#include <string>
#include <cstddef>
#include <unordered_set>

namespace ph
{

class SemanticVersion;

class SdlCommandGenerator
{
public:
	explicit SdlCommandGenerator(TSpanView<const SdlClass*> targetClasses);
	SdlCommandGenerator(TSpanView<const SdlClass*> targetClasses, const Path& sceneWorkingDirectory);
	virtual ~SdlCommandGenerator();

	// TODO: parameters like binary form? multi-thread?
	// TODO: command types, e.g., phantom

	virtual SdlDataPacketInterface& getPacketInterface();

	void generateLoadCommand(
		const ISdlResource* resource,
		std::string_view resourceName);

	void generateVersionCommand(const SemanticVersion& version);

	const Path& getSceneWorkingDirectory() const;
	void setSceneWorkingDirectory(const Path& directory);
	std::size_t numGeneratedCommands() const;
	std::size_t numGenerationErrors() const;
	void clearStats();
	bool hasTarget(const SdlClass* clazz) const;

protected:
	/*! @brief Called when the generator starts producing a command.
	@return Whether to generate command for this class.
	*/
	virtual bool beginCommand(
		const SdlClass* targetClass,
		SdlOutputContext* out_ctx) = 0;

	/*! @brief Called when the generator finishes producing a command.
	*/
	virtual void endCommand() = 0;

	/*! @brief Save target resource into output clauses.
	*/
	virtual void saveResource(
		const ISdlResource* resource,
		const SdlOutputContext& ctx,
		SdlOutputClauses& clauses) = 0;

	virtual void commandGenerated(
		std::string_view commandStr,
		const SdlOutputContext& ctx) = 0;

private:
	void generateLoadCommand(
		const ISdlResource& resource, 
		const SdlOutputContext& ctx,
		std::string_view resourceName,
		const SdlOutputClauses& clauses,
		std::string& out_commandStr);

	static void appendFullSdlType(
		const SdlClass* clazz,
		std::string& out_commandStr);

private:
	std::unordered_set<const SdlClass*> m_targetClasses;
	Path m_sceneWorkingDirectory;
	SdlInlinePacketInterface m_inlinePacketInterface;
	std::size_t m_numGeneratedCommands;
	std::size_t m_numGenerationErrors;
};

inline const Path& SdlCommandGenerator::getSceneWorkingDirectory() const
{
	return m_sceneWorkingDirectory;
}

inline std::size_t SdlCommandGenerator::numGeneratedCommands() const
{
	return m_numGeneratedCommands;
}

inline std::size_t SdlCommandGenerator::numGenerationErrors() const
{
	return m_numGenerationErrors;
}

}// end namespace ph
