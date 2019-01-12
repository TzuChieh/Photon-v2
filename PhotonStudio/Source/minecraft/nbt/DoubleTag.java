package minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

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
		m_value = BinaryData.readDouble(rawData);
		
		return Double.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "Double: " + getName() + " <" + m_value + ">";
	}
}
