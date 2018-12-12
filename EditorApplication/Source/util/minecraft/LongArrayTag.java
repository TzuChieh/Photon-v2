package util.minecraft;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.LongBuffer;

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
		int numArrayBytes = size * Long.BYTES;
		byte[] buffer = new byte[numArrayBytes];
		rawData.read(buffer);
		
		LongBuffer longBuffer = 
			ByteBuffer.wrap(buffer).
			order(ByteOrder.BIG_ENDIAN).
			asLongBuffer();
		
		m_array = new long[longBuffer.remaining()];
		longBuffer.get(m_array);
		
		return numArrayBytes + 4;
	}
}
