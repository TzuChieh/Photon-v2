package minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

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
		m_value = BinaryData.readLong(rawData);
		
		return Long.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "Long: " + getName() + " <" + m_value + ">";
	}
}
