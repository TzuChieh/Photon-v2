#include <ph_core.h>

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
	if(!phInit())
	{
		std::cerr << "Photon initialing failed" << std::endl;
		return EXIT_FAILURE;
	}

	PHuint64 engineId;
	phCreateEngine(&engineId, 1);

	std::ifstream sceneFile;
	sceneFile.open("../scene/default_scene.p2", std::ios::in);

	if(!sceneFile.is_open())
	{
		std::cerr << "scene file opening failed" << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		std::string lineCommand;
		while(sceneFile.good())
		{
			std::getline(sceneFile, lineCommand);
			lineCommand += '\n';
			phEnterCommand(engineId, lineCommand.c_str());
		}
		phEnterCommand(engineId, "->");

		sceneFile.close();
	}

	phRender(engineId);

	PHuint64 frameId;
	phCreateFrame(&frameId, PH_HDR_FRAME_TYPE);
	phDevelopFilm(engineId, frameId);

	if(!phExit())
	{
		std::cerr << "Photon exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}