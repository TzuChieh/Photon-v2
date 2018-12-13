package util.minecraft;

import java.io.IOException;
import java.io.InputStream;

import util.BinaryData;

public class MCAParser
{
	private static final int NUM_HEADER_BYTES = 8192;
	
	private ChunkData[][] m_chunks;
	
	public MCAParser(InputStream rawData)
	{
		m_chunks = new ChunkData[32][32];
		
		try
		{
			byte[] header = BinaryData.readByteArray(NUM_HEADER_BYTES, rawData);
			for(int chunkZ = 0; chunkZ < 32; ++chunkZ)
			{
				for(int chunkX = 0; chunkX < 32; ++chunkX)
				{
					int offset = 4 * (chunkZ * 32 + chunkX);
					int num4KiBOffsets = 
						((header[offset]     & 0xFF) << 16) | 
						((header[offset + 1] & 0xFF) << 8 ) | 
						((header[offset + 2] & 0xFF));
					int num4KiBSectors = header[offset + 3] & 0xFF;
					
					// TODO
				}
			}
		}
		catch(IOException e)
		{
			System.err.println("error parsing MCA, parsed data may be corrupted");
			e.printStackTrace();
		}
	}
}
