package util.minecraft;

public class ShortTag extends NBTTag
{
	private short m_value;
	
	public ShortTag()
	{
		m_value = 0;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public Short getPayload()
	{
		return m_value;
	}
	
	public void setValue(short value)
	{
		m_value = value;
	}
}
