package photonApi;

public final class FrameRegion
{
	private int   m_originXpx;
	private int   m_originYpx;
	private int   m_fullWidthPx;
	private int   m_fullHeightPx;
	private Frame m_regionedFrame;
	
	public FrameRegion(int originXpx, int originYpx, 
	                   int fullWidthPx, int fullHeightPx, 
	                   Frame regionedFrame)
	{
		set(originXpx, originYpx, 
		    fullWidthPx, fullHeightPx, 
		    regionedFrame);
	}
	
	public FrameRegion()
	{
		this(0, 0, 0, 0, null);
	}
	
	public Vector3f getRgb(int x, int y)
	{
		return m_regionedFrame.getRgb(x - m_originXpx, y - m_originYpx);
	}
	
	public void setRgb(int x, int y, Vector3f rgb)
	{
		m_regionedFrame.setRgb(x, y, rgb);
	}
	
	public void set(int originXpx, int originYpx, 
	                int fullWidthPx, int fullHeightPx, 
	                Frame regionedFrame)
	{
		m_originXpx     = originXpx;
		m_originYpx     = originYpx;
		m_fullWidthPx   = fullWidthPx;
		m_fullHeightPx  = fullHeightPx;
		m_regionedFrame = regionedFrame;
	}
	
	public void set(FrameRegion other)
	{
		set(other.m_originXpx, other.m_originYpx,
		    other.m_fullWidthPx, other.m_fullHeightPx, 
		    other.m_regionedFrame);
	}
	
	public Rectangle getRegion()
	{
		return new Rectangle(getOriginXpx(), getOriginYpx(), 
		                     getWidthPx(),   getHeightPx());
	}
	
	public boolean isValid()
	{
		return m_regionedFrame.isValid();
	}
	
	public int getOriginXpx()    { return m_originXpx;                   }
	public int getOriginYpx()    { return m_originYpx;                   }
	public int getWidthPx()      { return m_regionedFrame.getWidthPx();  }
	public int getHeightPx()     { return m_regionedFrame.getHeightPx(); }
	public int getFullWidthPx()  { return m_fullWidthPx;                 }
	public int getFullHeightPx() { return m_fullHeightPx;                }
	public int getNumComp()      { return m_regionedFrame.getNumComp();  }
}
