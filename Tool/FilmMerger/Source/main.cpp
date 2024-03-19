#include "ProcessedArguments.h"

#include <ph_core.h>
#include <Common/exceptions.h>
#include <Common/logging.h>
#include <DataIO/io_utils.h>
#include <Frame/RegularPicture.h>
#include <Frame/TFrame.h>
#include <Core/Filmic/HdrRgbFilm.h>
#include <Core/Filmic/SampleFilter.h>
#include <Math/TVector3.h>

#include <vector>
#include <iostream>

namespace
{

using namespace ph;

HdrRgbFilm make_film(const TFrame<float64, 3> frame, const float64 weight)
{
	HdrRgbFilm film(frame.widthPx(), frame.heightPx(), SampleFilter::makeBox());
	frame.forEachPixel(
		[weight, &film]
		(const uint32 x, const uint32 y, const TFrame<float64, 3>::PixelType& pixel)
		{
			film.setRgbPixel(x + 0.5, y + 0.5, math::Vector3D(pixel.toArray()), weight);
		});

	return film;
}

void merge_films(const film_merger::ProcessedArguments& args)
{
	std::vector<HdrRgbFilm> inputFilms;
	for(const film_merger::FilmInfo& info : args.getFilmInfos())
	{
		const RegularPicture picture = io_utils::load_picture(info.file);
		const TFrame<float64, 3> frame = picture.getPixels().toFrame<float64, 3>();

		const float64 weight = info.weight ? *info.weight : 1.0;
		if(!info.weight)
		{
			PH_DEFAULT_LOG(Note,
				"Loading film <{}> with weight = {}.", info.file, weight);
		}

		inputFilms.push_back(make_film(frame, weight));
	}

	if(inputFilms.empty())
	{
		PH_DEFAULT_LOG(Warning,
			"No film is gathered. No output will be generated.");
		return;
	}
	
	const auto mergedResPx = inputFilms.front().getActualResPx();
	PH_DEFAULT_LOG(Note,
		"Merging {} films, final resolution = {}.", inputFilms.size(), mergedResPx);

	HdrRgbFilm mergedFilm(mergedResPx.x(), mergedResPx.y(), SampleFilter::makeBlackmanHarris());
	for(std::size_t fi = 0; fi < inputFilms.size(); ++fi)
	{
		mergedFilm.mergeWith(inputFilms[fi]);

		PH_DEFAULT_LOG(Note,
			"merged film {}", fi);
	}

	HdrRgbFrame mergedFrame(static_cast<uint32>(mergedResPx.x()), static_cast<uint32>(mergedResPx.y()));
	mergedFilm.develop(mergedFrame);
	io_utils::save(mergedFrame, Path(args.getImageFilePath()));
}

}// end anonymous namespace

int main(int argc, char* argv[])
{
	if(argc <= 1)
	{
		std::cout << "Photon Film Merger\n";
		std::cout << "Use --help for a list of available commands.\n";
		return EXIT_SUCCESS;
	}

	if(!init_render_engine(EngineInitSettings::loadStandardConfig()))
	{
		return EXIT_FAILURE;
	}

	try
	{
		film_merger::ProcessedArguments args(argc, argv);
		if(args.isHelpRequested())
		{
			args.printHelpMessage();
		}
		else
		{
			merge_films(args);
		}
	}
	catch(const Exception& e)
	{
		PH_DEFAULT_LOG(Error,
			"Cannot merge films: {}", e.what());
	}

	return exit_render_engine() ? EXIT_SUCCESS : EXIT_FAILURE;
}
