package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

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
		m_value = (short)((rawData.read() << 8) | rawData.read());
		
		return Short.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "Short: " + getName() + " <" + m_value + ">";
	}
}
