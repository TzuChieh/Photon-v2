package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

public class DoubleTag extends NBTTag
{
	private double m_value;
	
	public DoubleTag()
	{
		m_value = 0;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public Double getPayload()
	{
		return m_value;
	}
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		long bits = 
			((long)(rawData.read()) << 56) | 
	    	((long)(rawData.read()) << 48) | 
	    	((long)(rawData.read()) << 40) | 
	    	((long)(rawData.read()) << 32) | 
	    	((long)(rawData.read()) << 24) | 
	    	((long)(rawData.read()) << 16) | 
	    	((long)(rawData.read()) << 8 ) | 
		 	((long)(rawData.read()));
	
		m_value = Double.longBitsToDouble(bits);
		
		return Double.BYTES;
	}
}
