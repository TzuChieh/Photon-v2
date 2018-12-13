package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

public class IntArrayTag extends NBTTag
{
	private int[] m_array;
	
	public IntArrayTag()
	{
		m_array = null;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public int[] getPayload()
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
		
		m_array = BinaryData.readIntArray(size, rawData);
		
		return size * Integer.BYTES + 4;
	}
}
