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
	public void generateData(StringBuilder out_sdlBuffer)
	{
		out_sdlBuffer.append(Float.toString(m_real));
	}
}
