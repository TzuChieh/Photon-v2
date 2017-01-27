package photonCore;

import photonApi.FloatRef;
import photonApi.LongRef;
import photonApi.Ph;

public final class PhRenderer
{
	protected final long m_rendererId;
	
	public PhRenderer(int numThreads)
	{
		LongRef rendererId = new LongRef();
		Ph.phCreateRenderer(rendererId, numThreads);
		m_rendererId = rendererId.m_value;
	}
	
	public void render(PhDescription description)
	{
		Ph.phRender(m_rendererId, description.m_descriptionId);
	}
	
	public float queryPercentageProgress()
	{
		FloatRef progress = new FloatRef();
		Ph.phQueryRendererPercentageProgress(m_rendererId, progress);
		return progress.m_value;
	}
	
	public float querySampleFrequency()
	{
		FloatRef frequency = new FloatRef();
		Ph.phQueryRendererSampleFrequency(m_rendererId, frequency);
		return frequency.m_value;
	}
}
