package photonCore;

import photonApi.FloatArrayRef;
import photonApi.IntRef;
import photonApi.LongRef;
import photonApi.Ph;

public final class PhFrame
{
	public static enum Type
	{
		HDR(Ph.PH_HDR_FRAME_TYPE);
		
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
	
	protected long m_frameId;
	
	public PhFrame(Type type)
	{
		LongRef frameId = new LongRef();
		Ph.phCreateFrame(frameId, type.getValue());
		m_frameId = frameId.m_value;
	}
	
	public void getData(FrameData data)
	{
		FloatArrayRef pixelData        = new FloatArrayRef();
		IntRef        widthPx          = new IntRef();
		IntRef        heightPx         = new IntRef();
		IntRef        nPixelComponents = new IntRef();
		Ph.phGetFrameData(m_frameId, pixelData, widthPx, heightPx, nPixelComponents);
		
		data.m_widthPx            = widthPx.m_value;
		data.m_heightPx           = heightPx.m_value;
		data.m_numPixelComponents = nPixelComponents.m_value;
		data.m_pixelData          = pixelData.m_value;
	}
	
	public void dispose()
	{
		Ph.phDeleteEngine(m_frameId);
	}
	
	@Override
	protected void finalize()
	{
		// TODO: check whether the frame is properly deleted, if not, deleted it
	}
}
