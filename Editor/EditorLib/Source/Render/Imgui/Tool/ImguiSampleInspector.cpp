#include "Render/Imgui/Tool/ImguiSampleInspector.h"
#include "Render/Imgui/Utility/imgui_helpers.h"

#include "ThirdParty/ImPlot.h"

#include <Common/primitive_type.h>
#include <Common/logging.h>
#include <Math/Random/TMt19937.h>
#include <Math/Random/Pcg32.h>
#include <Math/Random/Pcg64DXSM.h>
#include <Core/SampleGenerator/SGUniformRandom.h>
#include <Core/SampleGenerator/SGStratified.h>
#include <Core/SampleGenerator/SGHalton.h>

#include <algorithm>
#include <memory>
#include <format>
#include <utility>

namespace ph::editor
{

ImguiSampleInspector::ImguiSampleInspector()
	: isOpening(false)
	, m_scatterPlots()
	, m_plotNameBuffer(64, '\0')
	, m_numSamples(100)
	, m_sourceType(ESource::Rng)
	, m_useParamSafeguards(true)
	, m_autoClearOnGenerate(true)
	, m_paramSafeguardMessage()
	, m_rngSettings()
	, m_generatorSettings()

	, m_isNormalizedFitRequested(true)
{
	imgui::copy_to(m_plotNameBuffer, "samples");
}

void ImguiSampleInspector::buildWindow(const char* const title)
{
	// Auto center and determine a suitable size for first use
	ImGuiCond windowLayoutCond = ImGuiCond_FirstUseEver;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(
		viewport->GetCenter(),
		windowLayoutCond,
		ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(
		{viewport->WorkSize.x * 0.7f, viewport->WorkSize.y * 0.9f},
		windowLayoutCond);
	if(!ImGui::Begin(title, &isOpening))
	{
		ImGui::End();
		return;
	}

	// Left child: control panel
	ImGui::BeginChild(
		"sample_inspector_control_panel",
		ImVec2(std::max(ImGui::GetFontSize() * 20, ImGui::GetContentRegionAvail().x * 0.1f), 0),
		true,
		ImGuiWindowFlags_HorizontalScrollbar);
	buildControlPanelContent();
	ImGui::EndChild();

	ImGui::SameLine();

	// Right child: plotter view
	ImGui::BeginChild(
		"sample_inspector_plotter_view",
		ImVec2(0, 0),
		false,
		ImGuiWindowFlags_HorizontalScrollbar);
	buildPlotterViewContent();
	ImGui::EndChild();

	ImGui::End();
}

void ImguiSampleInspector::buildControlPanelContent()
{
	ImGui::RadioButton("RNG", &m_sourceType, ESource::Rng);
	ImGui::SameLine();
	ImGui::RadioButton("Generator", &m_sourceType, ESource::Generator);

	if(m_sourceType == ESource::Rng)
	{
		ImGui::Combo("Type", &m_rngSettings.type, RNG_NAMES, IM_ARRAYSIZE(RNG_NAMES));
		ImGui::InputInt("Seed", &m_rngSettings.seed);
		ImGui::InputInt("Sequence", &m_rngSettings.sequence);
	}
	else if(m_sourceType == ESource::Generator)
	{
		ImGui::Combo("Type", &m_generatorSettings.type, GENERATOR_NAMES, IM_ARRAYSIZE(GENERATOR_NAMES));
		ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
		ImGui::InputInt("X-axis Dimension", &m_generatorSettings.xAxisDimIndex, 1, 5);
		ImGui::InputInt("Y-axis Dimension", &m_generatorSettings.yAxisDimIndex, 1, 5);
		ImGui::PopItemWidth();
		ImGui::Checkbox("Use Sample Flow", &m_generatorSettings.useSampleFlow);
	}
	
	ImGui::InputText("Plot Name", m_plotNameBuffer.data(), m_plotNameBuffer.size());
	ImGui::Checkbox("Parameter Safeguards", &m_useParamSafeguards);
	ImGui::Checkbox("Clear On Generate", &m_autoClearOnGenerate);

	if(m_useParamSafeguards && !m_paramSafeguardMessage.empty())
	{
		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
		ImGui::TextWrapped(m_paramSafeguardMessage.c_str());
		ImGui::PopStyleColor();
		ImGui::Spacing();
	}

	bool canGenerate = false;
	if(ImGui::Button("Generate"))
	{
		if(m_useParamSafeguards)
		{
			safeguardParameters();
			canGenerate = m_paramSafeguardMessage.empty();
		}
		else
		{
			canGenerate = true;
		}
	}
	ImGui::SameLine();
	ImGui::InputInt("##num_samples", &m_numSamples, 50, 100);

	ImGui::Separator();

	ImGui::TextUnformatted("Plot Settings");
	if(ImGui::Button("Clear Data"))
	{
		m_scatterPlots.clear();
	}
	if(ImGui::Button("Normalized Fit"))
	{
		m_isNormalizedFitRequested = true;
	}

	if(canGenerate)
	{
		if(m_autoClearOnGenerate)
		{
			m_scatterPlots.clear();
		}

		ScatterPlotData plotData;
		plotData.name = m_plotNameBuffer.data();

		plotData.resize(m_numSamples);
		if(m_sourceType == ESource::Rng)
		{
			genRngPoints(plotData.xs, plotData.ys);
		}
		else if(m_sourceType == ESource::Generator)
		{
			genGeneratorPoints(plotData.xs, plotData.ys);
		}

		m_scatterPlots.push_back(plotData);
	}
}

void ImguiSampleInspector::buildPlotterViewContent()
{
	// Always a square plot, as large as available width or height (with a minimum size)
	const auto plotSize = std::max(
		ImGui::GetFontSize() * 20,
		std::min(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y));

	if(ImPlot::BeginPlot("##sample_inspector_scatter_plot", ImVec2(plotSize, plotSize)))
	{
		if(m_isNormalizedFitRequested)
		{
			ImPlot::SetupAxesLimits(0.0f, 1.0f, 0.0f, 1.0f, ImPlotCond_Always);
			m_isNormalizedFitRequested = false;
		}

		ImPlot::SetupFinish();

		for(const ScatterPlotData& plotData : m_scatterPlots)
		{
			ImPlot::PlotScatter(
				plotData.name.c_str(),
				plotData.xs.data(),
				plotData.ys.data(),
				plotData.getPointCount());
		}

		ImPlot::EndPlot();
	}
}

void ImguiSampleInspector::safeguardParameters()
{
	m_paramSafeguardMessage.clear();

	if(m_sourceType == ESource::Rng)
	{
		constexpr int maxSamples = 10000;
		if(m_numSamples <= 0 || m_numSamples > maxSamples)
		{
			m_paramSafeguardMessage += std::format(
				"* Number of RNG samples should be within [1, {}].\n",
				maxSamples);
		}
	}
	else if(m_sourceType == ESource::Generator)
	{
		constexpr int maxSamples = 2000;
		if(m_numSamples <= 0 || m_numSamples > maxSamples)
		{
			m_paramSafeguardMessage += std::format(
				"* Number of generator samples should be within [1, {}].\n",
				maxSamples);
		}

		constexpr int maxDimIndex = 50;
		if(m_generatorSettings.xAxisDimIndex < 0 || m_generatorSettings.xAxisDimIndex > maxDimIndex)
		{
			m_paramSafeguardMessage += std::format(
				"* X-axis dimension index should be within [0, {}].\n",
				maxDimIndex);
		}
		if(m_generatorSettings.yAxisDimIndex < 0 || m_generatorSettings.yAxisDimIndex > maxDimIndex)
		{
			m_paramSafeguardMessage += std::format(
				"* Y-axis dimension index should be within [0, {}].\n",
				maxDimIndex);
		}
	}
}

void ImguiSampleInspector::genRngPoints(
	const TSpan<float> out_xBuffer, 
	const TSpan<float> out_yBuffer) const
{
	if(m_rngSettings.type == ERng::MT_19937)
	{
		math::TMt19937<uint32> rng(m_rngSettings.seed);
		for(std::size_t i = 0; i < out_xBuffer.size(); ++i)
		{
			out_xBuffer[i] = rng.generateSample();
			out_yBuffer[i] = rng.generateSample();
		}
	}
	else if(m_rngSettings.type == ERng::PCG_32)
	{
		math::Pcg32 rng(m_rngSettings.sequence, m_rngSettings.seed);
		for(std::size_t i = 0; i < out_xBuffer.size(); ++i)
		{
			out_xBuffer[i] = rng.generateSample();
			out_yBuffer[i] = rng.generateSample();
		}
	}
	else if(m_rngSettings.type == ERng::PCG_64_DXSM)
	{
		math::Pcg64DXSM rng(0, m_rngSettings.sequence, 0, m_rngSettings.seed);
		for(std::size_t i = 0; i < out_xBuffer.size(); ++i)
		{
			out_xBuffer[i] = rng.generateSample();
			out_yBuffer[i] = rng.generateSample();
		}
	}
	else
	{
		PH_DEFAULT_LOG_WARNING(
			"Cannot generate points from RNG: unsupported type {}",
			m_rngSettings.type);
	}
}

void ImguiSampleInspector::genGeneratorPoints(
	const TSpan<float> out_xBuffer, 
	const TSpan<float> out_yBuffer) const
{
	std::unique_ptr<SampleGenerator> generator;
	switch(m_generatorSettings.type)
	{
	case EGenerator::UniformRandom:
		generator = std::make_unique<SGUniformRandom>(1);
		break;
		
	case EGenerator::Stratified:
		generator = std::make_unique<SGStratified>(1);
		break;
		
	case EGenerator::Halton:
		generator = std::make_unique<SGHalton>(1, EHaltonPermutation::None);
		break;

	case EGenerator::HaltonFixedScramble:
		generator = std::make_unique<SGHalton>(1, EHaltonPermutation::Fixed);
		break;

	case EGenerator::HaltonPerDigitScramble:
		generator = std::make_unique<SGHalton>(1, EHaltonPermutation::PerDigit);
		break;

	default:
		PH_DEFAULT_LOG_WARNING(
			"Cannot generate points from sample generator: unsupported type {}",
			m_generatorSettings.type);
		return;
	}

	const auto numSamples = out_xBuffer.size();
	
	const int xAxisDim = m_generatorSettings.xAxisDimIndex;
	const int yAxisDim = m_generatorSettings.yAxisDimIndex;
	const bool isConsecutiveDims = std::abs(xAxisDim - yAxisDim) == 1;
	const bool isRepeatedDims = xAxisDim == yAxisDim;
	const bool isReversedDims = xAxisDim > yAxisDim;

	// Skip dimensions till the first one we use
	{
		const int numDimsToSkip = std::min(xAxisDim, yAxisDim);
		if(numDimsToSkip >= 1)
		{
			generator->declareStageND(numDimsToSkip, 1);
		}
	}
	
	SamplesNDHandle handle1 = generator->declareStageND(
		isConsecutiveDims ? 2 : 1, 
		numSamples);

	SamplesNDHandle handle2 = handle1;
	if(!isConsecutiveDims && !isRepeatedDims)
	{
		// Skip dimensions till the second one we use
		const int numDimsToSkip = std::abs(xAxisDim - yAxisDim) - 1;
		PH_ASSERT_GE(numDimsToSkip, 1);
		generator->declareStageND(numDimsToSkip, 1);

		handle2 = generator->declareStageND(1, numSamples);
	}

	if(!generator->prepareSampleBatch())
	{
		PH_DEFAULT_LOG_WARNING(
			"Sample batch preparation failed.");
		return;
	}

	const bool useSampleFlow = m_generatorSettings.useSampleFlow;

	SamplesNDStream sampleStreams[] = {
		generator->getSamplesND(handle1), 
		generator->getSamplesND(handle2)};
	TSpan<float> out_buffers[] = {
		out_xBuffer, 
		out_yBuffer};
	for(std::size_t i = 0; i < out_xBuffer.size(); ++i)
	{
		// 2-D samples
		if(isConsecutiveDims)
		{
			if(useSampleFlow)
			{
				for(std::size_t di = 0; di < 2; ++di)
				{
					SampleFlow sampleFlow = sampleStreams[di].readSampleAsFlow();
					const auto sample2D = sampleFlow.flow2D();
					out_buffers[di][i] = sample2D[di];
				}
			}
			else
			{
				for(std::size_t di = 0; di < 2; ++di)
				{
					const auto sample2D = sampleStreams[di].readSample<2>();
					out_buffers[di][i] = sample2D[di];
				}
			}
		}
		// 1-D samples
		else
		{
			if(useSampleFlow)
			{
				for(std::size_t di = 0; di < 2; ++di)
				{
					SampleFlow sampleFlow = sampleStreams[di].readSampleAsFlow();
					const auto sample1D = sampleFlow.flow1D();
					out_buffers[di][i] = sample1D;
				}
			}
			else
			{
				for(std::size_t di = 0; di < 2; ++di)
				{
					const auto sample1D = sampleStreams[di].readSample<1>();
					out_buffers[di][i] = sample1D[0];
				}
			}
		}

		if(isReversedDims)
		{
			std::swap(out_buffers[0][i], out_buffers[1][i]);
		}
	}
}

}// end namespace ph::editor
