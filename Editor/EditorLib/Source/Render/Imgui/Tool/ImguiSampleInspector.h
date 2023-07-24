#pragma once

#include <Common/assertion.h>
#include <Utility/utility.h>
#include <Utility/TSpan.h>

#include <cstddef>
#include <vector>
#include <string>

namespace ph::editor
{

class ImguiSampleInspector final
{
public:
	bool isOpening;

	ImguiSampleInspector();

	void buildWindow(const char* title);

private:
	// Update `RNG_NAMES` if modified
	enum ERng : int
	{
		MT_19937 = 0,
		PCG_32,
		PCG_64_DXSM
	};

	static constexpr const char* RNG_NAMES[] =
	{
		"Mersenne Twister", 
		"PCG-32",
		"PCG-64-DXSM"
	};

	// Update `GENERATOR_NAMES` if modified
	enum EGenerator : int
	{
		UniformRandom = 0,
		Stratified,
		Halton
	};

	static constexpr const char* GENERATOR_NAMES[] =
	{
		"Uniform Random",
		"Stratified",
		"Halton"
	};

	enum ESource : int
	{
		Rng = 0,
		Generator
	};

	struct ScatterPlotData
	{
		std::string name;
		std::vector<float> xs;
		std::vector<float> ys;

		void clear();
		void resize(std::size_t size);
		void reserve(std::size_t size);
		int getPointCount() const;
	};

	struct RngSettings
	{
		int type = 0;
		int seed = 0;
		int sequence = 0;
	};

	struct GeneratorSettings
	{
		int type = 0;
		int xAxisDimIndex = 0;
		int yAxisDimIndex = 1;
		bool useSampleFlow = true;
	};

	void buildControlPanelContent();
	void buildPlotterViewContent();
	void safeguardParameters();
	void genRngPoints(TSpan<float> out_xBuffer, TSpan<float> out_yBuffer) const;
	void genGeneratorPoints(TSpan<float> out_xBuffer, TSpan<float> out_yBuffer) const;
	
	std::vector<ScatterPlotData> m_scatterPlots;
	std::vector<char> m_plotNameBuffer;
	int m_numSamples;
	int m_sourceType;
	bool m_useParamSafeguards;
	std::string m_paramSafeguardMessage;
	RngSettings m_rngSettings;
	GeneratorSettings m_generatorSettings;

	bool m_isNormalizedFitRequested;
};

inline void ImguiSampleInspector::ScatterPlotData::clear()
{
	name.clear();
	xs.clear();
	ys.clear();
}

inline void ImguiSampleInspector::ScatterPlotData::resize(const std::size_t size)
{
	xs.resize(size);
	ys.resize(size);
}

inline void ImguiSampleInspector::ScatterPlotData::reserve(const std::size_t size)
{
	xs.reserve(size);
	ys.reserve(size);
}

inline int ImguiSampleInspector::ScatterPlotData::getPointCount() const
{
	PH_ASSERT_EQ(xs.size(), ys.size());

	return lossless_cast<int>(xs.size());
}

}// end namespace ph::editor
