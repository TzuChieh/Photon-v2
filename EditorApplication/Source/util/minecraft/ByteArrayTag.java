package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

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
		int size = (rawData.read() << 24) | 
		           (rawData.read() << 16) | 
		           (rawData.read() << 8 ) | 
		           (rawData.read());
		m_array = new byte[size];
		rawData.read(m_array);
		
		return size + 4;
	}
}