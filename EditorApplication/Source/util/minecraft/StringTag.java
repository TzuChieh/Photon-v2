package util.minecraft;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

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
		byte[] stringBytes = new byte[numStringBytes];
		rawData.read(stringBytes);
		
		m_data = new String(stringBytes, StandardCharsets.UTF_8);
		
		return numStringBytes + 2;
	}
}
