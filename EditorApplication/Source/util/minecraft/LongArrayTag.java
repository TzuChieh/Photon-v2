package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

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
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		int size = 
			(rawData.read() << 24) | 
			(rawData.read() << 16) | 
			(rawData.read() << 8 ) | 
			(rawData.read());
		
		m_array = readLongArray(size, rawData);
		
		return size * Long.BYTES + 4;
	}
}