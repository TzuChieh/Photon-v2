package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

public class ShortTag extends NBTTag
{
	private short m_value;
	
	public ShortTag()
	{
		m_value = 0;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public Short getPayload()
	{
		return m_value;
	}
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		m_value = BinaryData.readShort(rawData);
		
		return Short.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "Short: " + getName() + " <" + m_value + ">";
	}
}
