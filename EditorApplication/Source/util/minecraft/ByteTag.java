package util.minecraft;

public class ByteTag extends NBTTag
{
	private byte m_value;
	
	public ByteTag()
	{
		m_value = 0;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public Byte getPayload()
	{
		return m_value;
	}
	
	public void setValue(byte value)
	{
		m_value = value;
	}
}
