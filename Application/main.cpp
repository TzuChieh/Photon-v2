#include <ph_core.h>

#include <iostream>
#include <fstream>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
	if(!phInit())
	{
		std::cerr << "Photon initialing failed" << std::endl;
		return EXIT_FAILURE;
	}

	PHuint64 engineId;
	phCreateEngine(&engineId, 10);

	std::ifstream sceneFile;
	sceneFile.open("../scene/default_scene.p2", std::ios::in);
	//sceneFile.open("../scene/test_bp.p2", std::ios::in);

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

	phUpdate(engineId);

	std::thread renderThread([=]()
	{
		phRender(engineId);
	});

	/*std::thread queryThread = [=]()
	{
		PHuint32 x, y, w, h;
		int regionStatus = phAsyncPollUpdatedFilmRegion(engineId, &x, &y, &w, &h);
		if(regionStatus != PH_FILM_REGION_STATUS_INVALID)
		{
			std::cout << "xywh: " << x << ", " << y << ", " << w << ", " << h << std::endl;
		}
	};*/

	renderThread.join();

	PHuint32 filmWpx, filmHpx;
	phGetFilmDimension(engineId, &filmWpx, &filmHpx);

	PHuint64 frameId;
	phCreateFrame(&frameId, filmWpx, filmHpx);
	phDevelopFilm(engineId, frameId);

	if(!phExit())
	{
		std::cerr << "Photon exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}