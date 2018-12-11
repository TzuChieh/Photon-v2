package util.minecraft;

public class LongArrayTag extends NBTTag
{
	private long[] m_array;
	
	public LongArrayTag()
	{
		m_array = null;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public long[] getPayload()
	{
		return m_array;
	}
	
	public void setArray(long[] array)
	{
		m_array = array;
	}
}
