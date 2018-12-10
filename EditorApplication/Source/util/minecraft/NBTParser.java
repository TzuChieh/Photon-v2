package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

public class NBTParser
{
	private NBTData m_data;
	
	public NBTParser(InputStream rawData)
	{
//		try
//		{
//			final int id = rawData.read();
//			while(id != -1)
//			{
//				switch(id)
//				{
//				case 0:
//					
//					break;
//				default:
//					System.err.println("warning: unknown id <" + id + ">, ignoring");
//					break;
//				}
//			}
//		}
//		catch(IOException e)
//		{
//			e.printStackTrace();
//		}
	}
	
	public NBTData getData()
	{
		return m_data;
	}
}
