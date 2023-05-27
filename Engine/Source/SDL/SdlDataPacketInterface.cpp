#include "SDL/SdlDataPacketInterface.h"

namespace ph
{

SdlDataPacketInterface::SdlDataPacketInterface(const Path& sceneWorkingDirectory)
	: m_sceneWorkingDirectory(sceneWorkingDirectory)
{}

SdlDataPacketInterface::~SdlDataPacketInterface() = default;

const Path& SdlDataPacketInterface::getSceneWorkingDirectory() const
{
	return m_sceneWorkingDirectory;
}

void SdlDataPacketInterface::setSceneWorkingDirectory(const Path& directory)
{
	m_sceneWorkingDirectory = directory;
}

}// end namespace ph
