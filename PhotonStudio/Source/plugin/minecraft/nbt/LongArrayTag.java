package plugin.minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

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
		int size = BinaryData.readInt(rawData);
		m_array = BinaryData.readLongArray(size, rawData);
		
		return size * Long.BYTES + Integer.BYTES;
	}
}
