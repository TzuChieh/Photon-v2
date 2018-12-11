package util.minecraft;

public class LongTag extends NBTTag
{
	private long m_value;
	
	public LongTag()
	{
		m_value = 0;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public Long getPayload()
	{
		return m_value;
	}
	
	public void setValue(long value)
	{
		m_value = value;
	}
}
