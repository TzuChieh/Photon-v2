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
	public String generateData()
	{
		return Integer.toString(m_integer);
	}
}
