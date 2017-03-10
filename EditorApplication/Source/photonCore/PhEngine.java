package photonCore;

import photonApi.FloatRef;
import photonApi.LongRef;
import photonApi.Ph;

public final class PhEngine
{
	protected final long m_engineId;
	
	public PhEngine(int numRenderThreads)
	{
		LongRef engineId = new LongRef();
		Ph.phCreateEngine(engineId, numRenderThreads);
		m_engineId = engineId.m_value;
	}
	
	public void enterCommand(String commandFragment)
	{
		Ph.phEnterCommand(m_engineId, commandFragment);
	}
	
	public void render()
	{
		Ph.phRender(m_engineId);
	}
	
	public void developFilm(PhFrame frame)
	{
		Ph.phDevelopFilm(m_engineId, frame.m_frameId);
	}
	
	public float queryPercentageProgress()
	{
		FloatRef progress = new FloatRef();
		Ph.phQueryRendererPercentageProgress(m_engineId, progress);
		return progress.m_value;
	}
	
	public float querySampleFrequency()
	{
		FloatRef frequency = new FloatRef();
		Ph.phQueryRendererSampleFrequency(m_engineId, frequency);
		return frequency.m_value;
	}
}
