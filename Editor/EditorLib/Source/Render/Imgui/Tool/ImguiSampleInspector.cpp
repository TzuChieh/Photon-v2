#include "Render/Imgui/Tool/ImguiSampleInspector.h"
#include "Render/Imgui/Editor/ImguiEditorUIProxy.h"

#include "ThirdParty/ImPlot.h"

#include <Common/primitive_type.h>
#include <Common/logging.h>
#include <Math/Random/TMt19937.h>
#include <Math/Random/Pcg32.h>
#include <Math/Random/Pcg64DXSM.h>

namespace ph::editor
{

ImguiSampleInspector::ImguiSampleInspector()
	: isOpening(false)
    , m_scatterPlots()
    , m_rngType(0)
    , m_rngSeed(0)
    , m_rngSequence(0)
{}

void ImguiSampleInspector::buildWindow(
    const char* const title,
    ImguiEditorUIProxy editorUI)
{
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if(!ImGui::Begin(title, &isOpening))
    {
        ImGui::End();
        return;
    }

    Editor& editor = editorUI.getEditor();

    ImGui::Combo("RNG Type", &m_rngType, RNG_NAMES, IM_ARRAYSIZE(RNG_NAMES));
    ImGui::InputInt("Seed", &m_rngSeed);
    ImGui::InputInt("Sequence", &m_rngSequence);

    if(ImGui::Button("Generate"))
    {
        ScatterPlotData plotData;
        plotData.resize(100);
        genRngPoints(plotData.xs, plotData.ys);

        m_scatterPlots.push_back(plotData);
    }

    if(ImPlot::BeginPlot("Scatter Plot"))
    {
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
