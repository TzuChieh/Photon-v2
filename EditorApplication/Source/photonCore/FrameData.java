package photonCore;

import java.util.Arrays;

public final class FrameData
{
	protected int     m_widthPx;
	protected int     m_heightPx;
	protected int     m_numPixelComponents;
	protected float[] m_pixelData;
	
	public FrameData()
	{
		m_widthPx            = 0;
		m_heightPx           = 0;
		m_numPixelComponents = 0;
		m_pixelData          = null;
	}
	
	public FrameData(FrameData other)
	{
		m_widthPx            = other.m_widthPx;
		m_heightPx           = other.m_heightPx;
		m_numPixelComponents = other.m_numPixelComponents;
		m_pixelData          = Arrays.copyOf(other.m_pixelData, other.m_pixelData.length);
	}
	
	public boolean isDataGood()
	{
		return m_widthPx > 0 && m_heightPx > 0 && m_numPixelComponents > 0 && m_pixelData != null;
	}
	
	public float[] getPixelData()
	{
		return m_pixelData;
	}
	
	public int getWidthPx()
	{
		return m_widthPx;
	}
	
	public int getHeightPx()
	{
		return m_heightPx;
	}
	
	public int getNumPixelComponents()
	{
		return m_numPixelComponents;
	}
}
