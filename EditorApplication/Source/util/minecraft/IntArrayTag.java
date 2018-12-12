package util.minecraft;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

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
		int numArrayBytes = size * Integer.BYTES;
		byte[] buffer = new byte[numArrayBytes];
		rawData.read(buffer);
		
		IntBuffer intBuffer = 
			ByteBuffer.wrap(buffer).
			order(ByteOrder.BIG_ENDIAN).
			asIntBuffer();
		
		m_array = new int[intBuffer.remaining()];
		intBuffer.get(m_array);
		
		return numArrayBytes + 4;
	}
}
