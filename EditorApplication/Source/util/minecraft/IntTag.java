package util.minecraft;

public class IntTag extends NBTTag
{
	private int m_value;
	
	public IntTag()
	{
		m_value = 0;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public Integer getPayload()
	{
		return m_value;
	}
	
	public void setValue(int value)
	{
		m_value = value;
	}
}
