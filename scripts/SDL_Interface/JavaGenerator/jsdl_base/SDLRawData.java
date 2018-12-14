package jsdl;

public class SDLRawData implements SDLData
{
	private String m_type;
	private String m_data;
	
	public SDLRawData(String type, String data)
	{
		m_type = type;
		m_data = data;
	}

	@Override
	public String getType()
	{
		return m_type;
	}

	@Override
	public String generateData()
	{
		return m_data;
	}
}
