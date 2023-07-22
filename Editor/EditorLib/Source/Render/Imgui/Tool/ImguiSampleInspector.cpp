#include "Render/Imgui/Tool/ImguiSampleInspector.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"
#include "Render/Imgui/Utility/imgui_helpers.h"

#include "ThirdParty/ImPlot.h"

#include <Common/primitive_type.h>
#include <Common/logging.h>
#include <Math/Random/TMt19937.h>
#include <Math/Random/Pcg32.h>
#include <Math/Random/Pcg64DXSM.h>

#include <algorithm>

namespace ph::editor
{

ImguiSampleInspector::ImguiSampleInspector()
	: isOpening(false)
	, m_scatterPlots()
	, m_plotNameBuffer(64, '\0')
	, m_numSamples(100)

	, m_rngType(0)
	, m_rngSeed(0)
	, m_rngSequence(0)

	, m_isNormalizedFitRequested(true)
{
	imgui::copy_to(m_plotNameBuffer, "samples");
}

void ImguiSampleInspector::buildWindow(
	const char* const title,
	ImguiEditorUIProxy editorUI)
{
	// Auto center and determine a suitable size when first use
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



	/*static float xs2[50], ys2[50];
	for(int i = 0; i < 50; i++) {
	xs2[i] = 0.25f + 0.2f * ((float)rand() / (float)RAND_MAX);
	ys2[i] = 0.75f + 0.2f * ((float)rand() / (float)RAND_MAX);
	}

	if(ImPlot::BeginPlot("Scatter Plot")) {
	ImPlot::PlotScatter("Data 1", m_pointXs.data(), m_pointYs.data(), m_pointXs.size());
	ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
	ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 6, ImPlot::GetColormapColor(1), IMPLOT_AUTO, ImPlot::GetColormapColor(1));
	ImPlot::PlotScatter("Data 2", xs2, ys2, 50);
	ImPlot::PopStyleVar();
	ImPlot::EndPlot();
	}*/

	ImGui::End();
}

void ImguiSampleInspector::buildControlPanelContent()
{
	ImGui::Combo("RNG Type", &m_rngType, RNG_NAMES, IM_ARRAYSIZE(RNG_NAMES));
	ImGui::InputInt("Seed", &m_rngSeed);
	ImGui::InputInt("Sequence", &m_rngSequence);
	ImGui::InputText("Plot Name", m_plotNameBuffer.data(), m_plotNameBuffer.size());
	if(ImGui::Button("Generate"))
	{
		ScatterPlotData plotData;
		plotData.name = m_plotNameBuffer.data();
		plotData.resize(m_numSamples);
		genRngPoints(plotData.xs, plotData.ys);

		m_scatterPlots.push_back(plotData);
	}
	ImGui::SameLine();
	ImGui::InputInt("##num_samples", &m_numSamples, 50);

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

void ImguiSampleInspector::genRngPoints(const TSpan<float> xBuffer, const TSpan<float> yBuffer) const
{
	if(m_rngType == ERng::MT_19937)
	{
		math::TMt19937<uint32> rng(m_rngSeed);
		for(std::size_t i = 0; i < xBuffer.size(); ++i)
		{
			xBuffer[i] = rng.generateSample();
			yBuffer[i] = rng.generateSample();
		}
	}
	else if(m_rngType == ERng::PCG_32)
	{
		math::Pcg32 rng(m_rngSequence, m_rngSeed);
		for(std::size_t i = 0; i < xBuffer.size(); ++i)
		{
			xBuffer[i] = rng.generateSample();
			yBuffer[i] = rng.generateSample();
		}
	}
	else if(m_rngType == ERng::PCG_64_DXSM)
	{
		math::Pcg64DXSM rng(0, m_rngSequence, 0, m_rngSeed);
		for(std::size_t i = 0; i < xBuffer.size(); ++i)
		{
			xBuffer[i] = rng.generateSample();
			yBuffer[i] = rng.generateSample();
		}
	}
	else
	{
		PH_DEFAULT_LOG_WARNING(
			"Cannot generate RNG points: invalid generator type {}",
			m_rngType);
	}
}

}// end namespace ph::editor
