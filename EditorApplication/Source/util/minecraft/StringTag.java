package util.minecraft;

public class StringTag extends NBTTag
{
	private String m_data;
	
	public StringTag()
	{
		m_data = "";
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public String getPayload()
	{
		return m_data;
	}
	
	public void setData(String data)
	{
		m_data = data;
	}
}
