package util.minecraft;

public class ByteArrayTag extends NBTTag
{
	private byte[] m_array;
	
	public ByteArrayTag()
	{
		m_array = null;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public byte[] getPayload()
	{
		return m_array;
	}
	
	public void setArray(byte[] array)
	{
		m_array = array;
	}
}
