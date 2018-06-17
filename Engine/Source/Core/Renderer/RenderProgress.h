#pragma once

#include "Common/primitive_type.h"
#include "Core/Renderer/Statistics.h"

namespace ph
{

class Renderer;

class RenderProgress final
{
public:
	Statistics statistics;

	RenderProgress(Renderer* renderer);

	/*void setTotalWork(uint32 totalWork);
	void setWorkDone(uint32 workDone);
	void setNumSamplesTaken(uint32 numSamples);
	void setNumMsElapsed(uint32 numMsElapsed);*/

private:
	Renderer* m_renderer;
	/*uint32 m_totalWork;
	uint32 m_workDone;
	uint32 m_numSamplesTaken;
	uint32 m_numMsElapsed;*/
};

// In-header Implementations:

//inline RenderProgress::RenderProgress() : 
//	m_totalWork(0), 
//	m_workDone(0), 
//	m_numSamplesTaken(0), 
//	m_numMsElapsed(0)
//{}
//
//inline void RenderProgress::setTotalWork(const uint32 totalWork)
//{
//	m_totalWork = totalWork;
//}
//
//inline void RenderProgress::setWorkDone(const uint32 workDone)
//{
//	m_workDone = workDone;
//}
//
//inline void RenderProgress::setNumSamplesTaken(const uint32 numSamples)
//{
//	m_numSamplesTaken = numSamples;
//}
//
//inline void RenderProgress::setNumMsElapsed(const uint32 numMsElapsed)
//{
//	m_numMsElapsed = numMsElapsed;
//}

}// end namespace ph