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
	
	public void getFullRgb(Frame out_frame)
	{
		FloatArrayRef rgbData = new FloatArrayRef();
		Ph.phCopyFrameRgbData(m_frameId, rgbData);
		if(rgbData.m_value == null)
		{
			System.err.println("bad raw data");
		}
		
		out_frame.set(m_widthPx, m_heightPx, 3, rgbData.m_value);
	}
	
	public FrameRegion copyRegionRgb(Rectangle region)
	{
		FloatArrayRef data = new FloatArrayRef();
		Ph.phCopyFrameRgbData(m_frameId, region.x, region.y, region.w, region.h, data);
		if(data.m_value == null)
		{
			System.err.println("bad region data");
		}
		
		Frame regionedFrame = new Frame(region.w, region.h, 3, data.m_value);
		FrameRegion frameRegion = new FrameRegion(region.x, region.y, 
		                                          m_widthPx, m_heightPx, 
		                                          regionedFrame);
		return frameRegion;
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
