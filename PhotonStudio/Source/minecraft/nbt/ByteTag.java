package minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

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
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		m_value = (byte)rawData.read();
		
		return Byte.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "Byte: " + getName() + " <" + m_value + ">";
	}
}
