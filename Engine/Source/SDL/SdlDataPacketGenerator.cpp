#include "SDL/SdlDataPacketGenerator.h"

namespace ph
{

SdlDataPacketGenerator::SdlDataPacketGenerator()
	: SdlDataPacketGenerator(Path("./"))
{}

SdlDataPacketGenerator::SdlDataPacketGenerator(Path sceneWorkingDirectory)
	: m_sceneWorkingDirectory(std::move(sceneWorkingDirectory))
{}

SdlDataPacketGenerator::~SdlDataPacketGenerator() = default;

void SdlDataPacketGenerator::generate(const ISdlResource& resource)
{
	// TODO
}

const Path& SdlDataPacketGenerator::getSceneWorkingDirectory() const
{
	return m_sceneWorkingDirectory;
}

void SdlDataPacketGenerator::setSceneWorkingDirectory(Path directory)
{
	m_sceneWorkingDirectory = std::move(directory);
}

}// end namespace ph
