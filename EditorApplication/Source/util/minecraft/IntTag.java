package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

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
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		m_value = 
			(rawData.read() << 24) | 
			(rawData.read() << 16) | 
			(rawData.read() << 8 ) | 
			(rawData.read());
		
		return Integer.BYTES;
	}
}
