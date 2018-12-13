package util.minecraft;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
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
		// TODO
		
		System.out.println(data);
		
		return new ChunkData();
	}
}
