package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

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
		int bits = 
			(rawData.read() << 24) | 
	     	(rawData.read() << 16) | 
	    	(rawData.read() << 8 ) | 
	    	(rawData.read());
		
		m_value = Float.intBitsToFloat(bits);
		
		return Float.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "Float: " + getName() + " <" + m_value + ">";
	}
}
