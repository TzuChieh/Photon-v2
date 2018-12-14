package jsdl;

public class SDLReal implements SDLData
{
	private float m_real;
	
	public SDLReal(float real)
	{
		m_real = real;
	}
	
	@Override
	public String getType()
	{
		return "real";
	}

	@Override
	public String generateData()
	{
		return Float.toString(m_real);
	}
}
