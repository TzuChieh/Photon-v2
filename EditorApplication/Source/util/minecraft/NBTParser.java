package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

public class NBTParser
{
	private NBTData m_data;
	
	public NBTParser(InputStream rawData)
	{
		// treating the file itself as a compound tag
		CompoundTag fileTag = new CompoundTag();
		
		// the content of the file is its payload
		try
		{
			fileTag.setPayload(rawData);
		}
		catch(IOException e)
		{
			System.err.println("error during parsing, parsed data may be corrupted");
			e.printStackTrace();
		}
		
		m_data = fileTag.getPayload();
	}
	
	public NBTData getData()
	{
		return m_data;
	}
}
