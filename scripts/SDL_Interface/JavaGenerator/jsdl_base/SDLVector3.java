package jsdl;

public class SDLVector3 implements SDLData
{
	private float m_x;
	private float m_y;
	private float m_z;
	
	public SDLVector3()
	{
		this(0.0f, 0.0f, 0.0f);
	}
	
	public SDLVector3(float x, float y, float z)
	{
		m_x = x;
		m_y = y;
		m_z = z;
	}

	@Override
	public String getType()
	{
		return "vector3";
	}

	@Override
	public String generateData()
	{
		return String.format("\"%f %f %f\"", m_x, m_y, m_z);
	}
}
