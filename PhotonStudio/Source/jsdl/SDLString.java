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
	public void generateData(StringBuilder out_sdlBuffer)
	{
		out_sdlBuffer.append(m_string);
	}
}
