#include "ProcessedArguments.h"
#include "util.h"
#include "StaticImageRenderer.h"

#include <ph_core.h>

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <cmath>

// FIXME: add osx fs headers once it is supported
#if defined(_WIN32)
	#include <filesystem>
#elif defined(__linux__)
	#include <experimental/filesystem>
#endif

int main(int argc, char* argv[])
{
	if(argc <= 1)
	{
		std::cout << "Photon-v2 Renderer" << std::endl;
		std::cout << "Use --help for a list of available commands." << std::endl;
		return EXIT_SUCCESS;
	}

	ProcessedArguments args(argc, argv);
	if(args.isHelpMessageRequested())
	{
		ProcessedArguments::printHelpMessage();
		return EXIT_SUCCESS;
	}

	if(!phInit())
	{
		std::cerr << "Photon initialing failed" << std::endl;
		return EXIT_FAILURE;
	}

	// begin engine operations

	// HACK
	if(args.isFrameDiagRequested())
	{
		std::cout << "begin frame diag" << std::endl;

		PHuint64 frameA, frameB, resultFrame;
		phCreateFrame(&frameA, 0, 0);
		phCreateFrame(&frameB, 0, 0);
		phCreateFrame(&resultFrame, 0, 0);

		phLoadFrame(frameA, args.getFramePathA().c_str());
		phLoadFrame(frameB, args.getFramePathB().c_str());

		phFrameOpAbsDifference(frameA, frameB, resultFrame);
		phSaveFrame(resultFrame, "./frame_diag_abs_diff.exr");

		const float MSE = phFrameOpMSE(frameA, frameB);
		{
			std::ostream transientCout(std::cout.rdbuf());
			transientCout << std::setprecision(20) << "MSE  = " << MSE << std::endl;
			transientCout << std::setprecision(20) << "RMSE = " << std::sqrt(MSE) << std::endl;
		}

		std::cout << "end frame diag" << std::endl;
	}

	if(!args.isImageSeriesRequested())
	{
		StaticImageRenderer renderer(args);
		renderer.render();
	}
	else
	{
		renderImageSeries(args);
	}

	// end engine operations

	if(!phExit())
	{
		std::cerr << "Photon exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
