package photonCore;

import photonApi.FloatRef;
import photonApi.LongRef;
import photonApi.Ph;

public final class PhRenderer
{
	public static enum Type
	{
		PREVIEW       (Ph.PH_PREVIEW_RENDERER_TYPE), 
		IMPORTANCE    (Ph.PH_IMPORTANCE_RENDERER_TYPE), 
		MT_IMPORTANCE (Ph.PH_MT_IMPORTANCE_RENDERER_TYPE);
		
		private final int m_phValue;
		
		private Type(int phValue)
		{
			m_phValue = phValue;
		}
		
		public int getValue()
		{
			return m_phValue;
		}
	}
	
	protected final long m_rendererId;
	
	public PhRenderer(Type type, int numThreads)
	{
		LongRef rendererId = new LongRef();
		Ph.phCreateRenderer(rendererId, type.getValue(), numThreads);
		m_rendererId = rendererId.m_value;
	}
	
	public void render(PhWorld world, PhCamera camera)
	{
		Ph.phRender(m_rendererId, world.m_worldId, camera.m_cameraId);
	}
	
	public void setSampleGenerator(PhSampleGenerator sampleGenerator)
	{
		Ph.phSetRendererSampleGenerator(m_rendererId, sampleGenerator.m_sampleGeneratorId);
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
