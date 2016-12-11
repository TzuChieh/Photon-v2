package photonCore;

import photonApi.LongRef;
import photonApi.Ph;

public final class PhSampleGenerator
{
	public static enum Type
	{
		PIXEL_JITTER(Ph.PH_PIXEL_JITTER_SAMPLE_GENERATOR_TYPE);
		
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
	
	protected long m_sampleGeneratorId;
	
	public PhSampleGenerator(Type type, int sppBudget)
	{
		LongRef sampleGeneratorId = new LongRef();
		Ph.phCreateSampleGenerator(sampleGeneratorId, type.getValue(), sppBudget);
		m_sampleGeneratorId = sampleGeneratorId.m_value;
	}
}
