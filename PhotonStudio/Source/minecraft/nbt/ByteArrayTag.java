package minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

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
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		int size = BinaryData.readInt(rawData);
		m_array = BinaryData.readByteArray(size, rawData);
		
		return size * Byte.BYTES + Integer.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "Byte Array: " + getName() + " <" + m_array + ">";
	}
}
