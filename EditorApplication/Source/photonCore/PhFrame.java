package photonCore;

import photonApi.FloatArrayRef;
import photonApi.IntRef;
import photonApi.LongRef;
import photonApi.Ph;
import photonCore.exception.PhDataInconsistentException;

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
	
	private int m_widthPx;
	private int m_heightPx;
	
	public PhFrame(Type type, int widthPx, int heightPx)
	{
		m_widthPx  = widthPx;
		m_heightPx = heightPx;
		
		LongRef frameId = new LongRef();
		Ph.phCreateFrame(frameId, type.getValue(), widthPx, heightPx);
		m_frameId = frameId.m_value;
	}
	
	public void getData(FrameData data) throws PhDataInconsistentException
	{
		FloatArrayRef pixelData        = new FloatArrayRef();
		IntRef        widthPx          = new IntRef();
		IntRef        heightPx         = new IntRef();
		IntRef        nPixelComponents = new IntRef();
		Ph.phGetFrameData(m_frameId, pixelData, widthPx, heightPx, nPixelComponents);
		
		// check consistency
		if(m_widthPx != widthPx.m_value || m_heightPx != heightPx.m_value)
		{
			throw new PhDataInconsistentException("frame data has inconsistent dimension: "
			                                    + "cached<" + m_widthPx + ", " + m_heightPx + "> actual<" + widthPx + ", " + heightPx + ">");
		}
		
		data.m_widthPx            = m_widthPx;
		data.m_heightPx           = m_heightPx;
		data.m_numPixelComponents = nPixelComponents.m_value;
		data.m_pixelData          = pixelData.m_value;
	}
}
