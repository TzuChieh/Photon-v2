package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

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
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		m_value =
			((long)(rawData.read()) << 56) | 
			((long)(rawData.read()) << 48) | 
			((long)(rawData.read()) << 40) | 
			((long)(rawData.read()) << 32) | 
			((long)(rawData.read()) << 24) | 
			((long)(rawData.read()) << 16) | 
			((long)(rawData.read()) << 8 ) | 
			((long)(rawData.read()));
		
		return Long.BYTES;
	}
}
