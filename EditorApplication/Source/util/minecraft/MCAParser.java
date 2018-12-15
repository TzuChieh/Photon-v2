package util.minecraft;

import java.io.ByteArrayInputStream;
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
		
		ChunkParser chunkParser = new ChunkParser();
		try
		{
			byte[] header    = BinaryData.readByteArray(NUM_HEADER_BYTES, rawData);
			byte[] remaining = BinaryData.readAll(rawData);
			for(int chunkZ = 0; chunkZ < 32; ++chunkZ)
			{
				for(int chunkX = 0; chunkX < 32; ++chunkX)
				{
//					System.out.println("x: " + chunkX + ", z: " + chunkZ);
					
					int offset = 4 * (chunkZ * 32 + chunkX);
					int num4KiBOffsets = 
						((header[offset]     & 0xFF) << 16) | 
						((header[offset + 1] & 0xFF) << 8 ) | 
						((header[offset + 2] & 0xFF));
					int num4KiBSectors = header[offset + 3] & 0xFF;
					
					// this means the chunk data is not present (has not been generated or migrated)
					if(num4KiBOffsets == 0 && num4KiBSectors == 0)
					{
						continue;
					}
					
					int chunkDataOffset = (num4KiBOffsets - 2) * 4096;
					int chunkSize       = num4KiBSectors * 4096;
					ByteArrayInputStream chunkData = new ByteArrayInputStream(remaining, chunkDataOffset, chunkSize);
					m_chunks[chunkZ][chunkX] = chunkParser.parse(chunkData);
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
