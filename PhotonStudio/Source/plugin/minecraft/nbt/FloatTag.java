package plugin.minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

public class FloatTag extends NBTTag
{
	private float m_value;
	
	public FloatTag()
	{
		m_value = 0;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public Float getPayload()
	{
		return m_value;
	}
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		m_value = BinaryData.readFloat(rawData);
		
		return Float.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "Float: " + getName() + " <" + m_value + ">";
	}
}
