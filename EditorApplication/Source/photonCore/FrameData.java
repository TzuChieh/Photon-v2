package photonCore;

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
