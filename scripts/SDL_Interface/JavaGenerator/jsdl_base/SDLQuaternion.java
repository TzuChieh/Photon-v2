package jsdl;

public class SDLQuaternion implements SDLData
{
	private float m_x;
	private float m_y;
	private float m_z;
	private float m_w;
	
	public SDLQuaternion()
	{
		this(0.0f, 0.0f, 0.0f, 1.0f);
	}
	
	public SDLQuaternion(float x, float y, float z, float w)
	{
		m_x = x;
		m_y = y;
		m_z = z;
		m_w = w;
	}

	@Override
	public String getType()
	{
		return "quaternion";
	}

	@Override
	public String generateData()
	{
		return String.format("\"%f %f %f %f\"", m_x, m_y, m_z, m_w);
	}
}
