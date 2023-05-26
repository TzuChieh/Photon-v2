#include "SDL/SdlDataPacketGenerator.h"

namespace ph
{

SdlDataPacketGenerator::SdlDataPacketGenerator(Path sceneWorkingDirectory)
	: m_sceneWorkingDirectory(std::move(sceneWorkingDirectory))
{}

SdlDataPacketGenerator::~SdlDataPacketGenerator() = default;

const Path& SdlDataPacketGenerator::getSceneWorkingDirectory() const
{
	return m_sceneWorkingDirectory;
}

void SdlDataPacketGenerator::setSceneWorkingDirectory(Path directory)
{
	m_sceneWorkingDirectory = std::move(directory);
}

}// end namespace ph
