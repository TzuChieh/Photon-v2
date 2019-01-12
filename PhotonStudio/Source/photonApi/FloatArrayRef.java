package photonApi;

public class FloatArrayRef
{
	public float[] m_value;
	
	public void setLength(int length)
	{
		m_value = new float[length];
	}
	
	public int getLength()
	{
		return m_value != null ? m_value.length : 0;
	}
}
