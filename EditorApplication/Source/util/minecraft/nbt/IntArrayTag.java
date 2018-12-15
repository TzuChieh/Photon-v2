package util.minecraft.nbt;

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
		int size = BinaryData.readInt(rawData);
		m_array = BinaryData.readIntArray(size, rawData);
		
		return size * Integer.BYTES + 4;
	}
}
