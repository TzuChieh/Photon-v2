package photonApi;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public final class PhFrame
{
	protected long m_frameId;
	protected int  m_widthPx;
	protected int  m_heightPx;
	
	public PhFrame(int widthPx, int heightPx)
	{
		LongRef frameId = new LongRef();
		Ph.phCreateFrame(frameId, widthPx, heightPx);
		
		m_frameId  = frameId.m_value;
		m_widthPx  = widthPx;
		m_heightPx = heightPx;
	}
	
	public void getRgbData(FrameData data)
	{
		data.m_widthPx            = m_widthPx;
		data.m_heightPx           = m_heightPx;
		data.m_numPixelComponents = 3;
		data.m_rgbData            = getRawRgbData();
	}
	
	public float[] getRawRgbData()
	{
		FloatArrayRef rgbData = new FloatArrayRef();
		Ph.phCopyFrameRgbData(m_frameId, rgbData);
		if(rgbData.m_value == null)
		{
			System.err.println("bad raw data");
		}
		
		return rgbData.m_value;
	}
	
	public void dispose()
	{
		Ph.phDeleteFrame(m_frameId);
	}
	
	public int widthPx()
	{
		return m_widthPx;
	}
	
	public int heightPx()
	{
		return m_heightPx;
	}
	
	@Override
	protected void finalize()
	{
		// TODO: check whether the frame is properly deleted, if not, delete it
		// and emit some warning message
	}
}
