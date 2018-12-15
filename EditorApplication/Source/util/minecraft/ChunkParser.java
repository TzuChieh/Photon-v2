package util.minecraft;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.zip.GZIPInputStream;
import java.util.zip.InflaterInputStream;

import util.BinaryData;

public class ChunkParser
{
	private static final int GZIP = 1;
	private static final int ZLIB = 2;
	
	public ChunkData parse(InputStream rawData)
	{
		ChunkData chunk = null;
		
		try
		{
			int    numBytes        = BinaryData.readInt(rawData);
			int    compressionType = rawData.read();
			byte[] compressedChunk = BinaryData.readByteArray(numBytes - 1, rawData);
			
			InputStream decompressedChunk = null;
			if(compressionType == ZLIB)
			{
				decompressedChunk = new InflaterInputStream(new ByteArrayInputStream(compressedChunk));
				
			}
			else if(compressionType == GZIP)
			{
				decompressedChunk = new GZIPInputStream(new ByteArrayInputStream(compressedChunk));
			}
			else
			{
				System.err.println(
					"unknown compression type <" + compressionType + ">, assuming uncompressed");
				decompressedChunk = new ByteArrayInputStream(compressedChunk);
			}
			
			chunk = parseChunkNBT(new NBTParser(decompressedChunk).getData());
		}
		catch(IOException e)
		{
			System.err.println("error parsing chunk, parsed data may be corrupted");
			e.printStackTrace();
		}
		
		return chunk;
	}
	
	private ChunkData parseChunkNBT(NBTData data)
	{
		NBTData root  = data.get("").getPayload();
		NBTData level = root.get("Level").getPayload();
		
		int xPos = level.get("xPos").getPayload();
		int zPos = level.get("zPos").getPayload();
		
		ChunkData chunkData = new ChunkData(xPos, zPos);
		
		List<NBTData> sections = level.get("Sections").getPayload();
		for(NBTData section : sections)
		{
			byte y = section.get("Y").getPayload();
			List<NBTData> palette = section.get("Palette").getPayload();
			
			for(NBTData blockStates : palette)
			{
				String blockIdName = blockStates.get("Name").getPayload();
				
				System.out.println(blockIdName);
				
				if(blockStates.has("Properties"))
				{
					NBTData stateProperties = blockStates.get("Properties").getPayload();
					System.out.println(stateProperties);
				}
				
			}
		}
		
		System.err.println("x: " + xPos + ", z: " + zPos);
		
		return chunkData;
	}
}
