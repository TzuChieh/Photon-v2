package minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

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
		m_value = BinaryData.readInt(rawData);
		
		return Integer.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "Int: " + getName() + " <" + m_value + ">";
	}
}
