package photonApi;

import util.Vector3f;

public final class Frame
{
	private int     m_widthPx;
	private int     m_heightPx;
	private int     m_numComp;
	private float[] m_data;
	
	public Frame(int widthPx, int heightPx, int numComp)
	{
		this(widthPx, heightPx, numComp, 
		     new float[widthPx * heightPx * numComp]);
	}
	
	public Frame(int widthPx, int heightPx, int numComp, float[] data)
	{
		set(widthPx, heightPx, numComp, data);
	}
	
	public Frame()
	{
		this(0, 0, 0, new float[0]);
	}
	
	public Vector3f getRgb(int x, int y)
	{
		int baseIndex = (y * m_widthPx + x) * 3;
		
		return new Vector3f(m_data[baseIndex + 0], 
		                    m_data[baseIndex + 1],
		                    m_data[baseIndex + 2]);
	}
	
	public void setRgb(int x, int y, Vector3f rgb)
	{
		int baseIndex = (y * m_widthPx + x) * 3;
		
		m_data[baseIndex + 0] = rgb.x;
		m_data[baseIndex + 1] = rgb.y;
		m_data[baseIndex + 2] = rgb.z;
	}
	
	public void set(int widthPx, int heightPx, int numComp, float[] data)
	{
		m_widthPx  = widthPx;
		m_heightPx = heightPx;
		m_numComp  = numComp;
		m_data     = data;
	}
	
	public boolean isValid()
	{
		return m_data.length != 0;
	}
	
	public int     getWidthPx()  { return m_widthPx;  }
	public int     getHeightPx() { return m_heightPx; }
	public int     getNumComp()  { return m_numComp;  }
	public float[] getData()     { return m_data;     }
}
