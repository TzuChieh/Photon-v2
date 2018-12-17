package jsdl;

public class SDLInteger implements SDLData
{
	private int m_integer;
	
	public SDLInteger(int integer)
	{
		m_integer = integer;
	}

	@Override
	public String getType()
	{
		return "integer";
	}

	@Override
	public void generateData(StringBuilder out_sdlBuffer)
	{
		out_sdlBuffer.append(Integer.toString(m_integer));
	}
}
