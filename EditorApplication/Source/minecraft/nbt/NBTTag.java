package minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

public abstract class NBTTag
{
	private String m_name;
	private int    m_numTagBytes;
	private int    m_numPayloadBytes;
	
	protected NBTTag()
	{
		m_name            = "";
		m_numTagBytes     = 0;
		m_numPayloadBytes = 0;
	}
	
	public abstract <T> T getPayload();
	public abstract int setPayload(InputStream rawData) throws IOException;
	
	public NBTTag fromRawData(InputStream rawData) throws IOException
	{
		final int NUM_ID_BYTES          = Byte.BYTES;
		final int NUM_NAME_LENGTH_BYTES = Short.BYTES;
		
		if(this instanceof EndTag)
		{
			setName("");
			m_numPayloadBytes = setPayload(rawData);
			m_numTagBytes = NUM_ID_BYTES + m_numPayloadBytes;
			return this;
		}
		
		short numNameBytes = BinaryData.readShort(rawData);
		String tagName = BinaryData.readStringUTF8(numNameBytes, rawData);
		setName(tagName);
		
		m_numPayloadBytes = setPayload(rawData);
		m_numTagBytes = 
			NUM_ID_BYTES + 
			NUM_NAME_LENGTH_BYTES + 
			numNameBytes + 
			m_numPayloadBytes;
		
		return this;
	}
	
	public String getName()
	{
		return m_name;
	}
	
	public NBTTag setName(String name)
	{
		m_name = name;
		
		return this;
	}
	
	public int numTagBytes()
	{
		return m_numTagBytes;
	}
	
	public int numPayloadBytes()
	{
		return m_numPayloadBytes;
	}
}
