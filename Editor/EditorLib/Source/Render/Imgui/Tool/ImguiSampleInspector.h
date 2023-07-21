#pragma once

#include <Common/assertion.h>
#include <Utility/utility.h>
#include <Utility/TSpan.h>

#include <cstddef>
#include <vector>
#include <string>
#include <string>

namespace ph::editor
{

class ImguiEditorUIProxy;

class ImguiSampleInspector final
{
public:
	bool isOpening;

	ImguiSampleInspector();

	void buildWindow(
		const char* title, 
		ImguiEditorUIProxy editorUI);

private:
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

	void genRngPoints(TSpan<float> out_xBuffer, TSpan<float> out_yBuffer) const;
	
	std::vector<ScatterPlotData> m_scatterPlots;
	int m_rngType;
	int m_rngSeed;
	int m_rngSequence;
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
