package jsdl;

public class SDLReference implements SDLData
{
	private String m_type;
	private String m_name;
	
	public SDLReference(String type, String name)
	{
		m_type = type;
		m_name = name;
	}
	
	@Override
	public String getType()
	{
		return m_type;
	}

	@Override
	public String generateData()
	{
		return "\"@" + m_name + "\"";
	}
}
