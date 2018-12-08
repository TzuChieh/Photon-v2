package jsdl;

public class SDLString implements SDLData
{
	private String m_string;
	
	public SDLString(String string)
	{
		m_string = string;
	}
	
	@Override
	public String getType()
	{
		return "string";
	}

	@Override
	public String generateData()
	{
		return m_string;
	}
}
