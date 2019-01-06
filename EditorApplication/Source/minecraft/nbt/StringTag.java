package minecraft.nbt;

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
		short numStringBytes = BinaryData.readShort(rawData);
		m_data = BinaryData.readStringUTF8(numStringBytes, rawData);
		
		return numStringBytes + Short.BYTES;
	}
	
	@Override
	public String toString()
	{
		return "String: " + getName() + " <" + m_data + ">";
	}
}
