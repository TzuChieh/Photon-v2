#pragma once

#include "SDL/sdl_fwd.h"
#include "DataIO/FileSystem/Path.h"
#include "SDL/SdlDataPacketInterface.h"
#include "Utility/TSpan.h"

#include <string_view>
#include <string>
#include <cstddef>
#include <unordered_set>
#include <memory>
#include <vector>
#include <utility>

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

	void generateResourceCommand(
		const ISdlResource* resource,
		std::string_view resourceName);

	void generateCachedNamedDataPacketCommand(
		const SdlNamedOutputClauses& namedClauses);

	void generateVersionCommand(const SemanticVersion& version);

	void setPacketInterface(std::unique_ptr<SdlDataPacketInterface> interface);
	void setSceneWorkingDirectory(const Path& directory);
	SdlDataPacketInterface& getPacketInterface();
	const Path& getSceneWorkingDirectory() const;
	std::size_t numGeneratedCommands() const;
	std::size_t numGenerationErrors() const;
	void clearStats();
	bool hasTarget(const SdlClass* clazz) const;

protected:
	/*! @brief Called when the generator starts producing a command.
	@param[out] out_ctx Stores the context for the command. This context will be used for subsequent
	handlers (`saveResource()`, `commandGenerated()`, `commandGenerated()`) for the same command.
	@return Whether to generate command for this class. If `false`, this command will be skipped and
	`endCommand()` will not be called.
	*/
	virtual bool beginCommand(
		const SdlClass* targetClass,
		SdlOutputContext* out_ctx) = 0;

	/*! @brief Called when the generator finishes producing a command.
	This is called last in the sequence of handler calls for a command. Will not be called on error
	or if the command is canceled (see `beginCommand()`).
	*/
	virtual void endCommand() = 0;

	/*! @brief Save target resource into output clauses.
	@param resource The resource to save.
	@param ctx Context for the command.
	@param[out] clauses Buffer to append the generated clauses.
	*/
	virtual void saveResource(
		const ISdlResource* resource,
		const SdlOutputContext& ctx,
		SdlOutputClauses& clauses) = 0;

	/*! @brief Called when one or more commands are generated.
	@param commandStr The newly generated command(s).
	@param ctx Context for the command.
	*/
	virtual void commandGenerated(
		std::string_view commandStr,
		const SdlOutputContext& ctx) = 0;

private:
	/*! @brief Generate a load command for the resource.
	*/
	void generateLoadCommand(
		const ISdlResource& resource, 
		const SdlOutputContext& ctx,
		std::string_view resourceName,
		const SdlOutputClauses& clauses,
		std::string& out_commandStr);

	std::string borrowStringBuffer();
	void returnStringBuffer(std::string&& buffer);

	static void appendFullSdlType(
		const SdlClass* clazz,
		std::string& out_commandStr);

private:
	std::unordered_set<const SdlClass*> m_targetClasses;
	std::unique_ptr<SdlDataPacketInterface> m_packetInterface;
	Path m_sceneWorkingDirectory;
	std::size_t m_numGeneratedCommands;
	std::size_t m_numGenerationErrors;
	std::vector<std::string> m_stringBuffers;
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

inline std::string SdlCommandGenerator::borrowStringBuffer()
{
	if(m_stringBuffers.empty())
	{
		m_stringBuffers.push_back(std::string());
	}

	std::string buffer = std::move(m_stringBuffers.back());
	m_stringBuffers.pop_back();
	return buffer;
}

inline void SdlCommandGenerator::returnStringBuffer(std::string&& buffer)
{
	buffer.clear();
	m_stringBuffers.push_back(std::move(buffer));
}

}// end namespace ph
