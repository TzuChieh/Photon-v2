package util.minecraft;

public abstract class NBTTag
{
	private String m_name;
	
	protected NBTTag()
	{
		m_name = "";
	}
	
	public abstract <T> T getPayload();
	
	public String getName()
	{
		return m_name;
	}
	
	public void setName(String name)
	{
		m_name = name;
	}
}
