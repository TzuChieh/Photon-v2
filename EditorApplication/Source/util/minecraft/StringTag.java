package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

public class StringTag extends NBTTag
{
	private String m_data;
	
	public StringTag()
	{
		m_data = "";
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public String getPayload()
	{
		return m_data;
	}
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		int numStringBytes = ((rawData.read() << 8) | rawData.read());
		
		m_data = BinaryData.readStringUTF8(numStringBytes, rawData);
		
		return numStringBytes + 2;
	}
	
	@Override
	public String toString()
	{
		return "String: " + getName() + " <" + m_data + ">";
	}
}
