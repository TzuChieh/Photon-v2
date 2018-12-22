package minecraft.parser;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

import minecraft.RegionData;
import util.BinaryData;

public class MCAParser
{
	private static final int NUM_HEADER_BYTES = 8192;
	
	private ChunkParser m_chunkParser;
	
	public MCAParser()
	{
		m_chunkParser = new ChunkParser();
	}
	
	// TODO: use Path
	public RegionData parse(File mcaFile)
	{
		RegionData region = null;
		try
		{
			InputStream input    = new FileInputStream(mcaFile);
			String      filename = mcaFile.getName();
			
			String[] tokens = filename.split("[.]");
			if(tokens.length == 4 && tokens[0].equals("r") && tokens[3].equals("mca"))
			{
				int regionX = Integer.valueOf(tokens[1]);
				int regionZ = Integer.valueOf(tokens[2]);
				region = parse(regionX, regionZ, input);
			}
			else
			{
				System.err.println("unknown format of the MCA filename <" + filename + ">, region not parsed");
			}
		}
		catch(FileNotFoundException e)
		{
			System.err.println("error opening MCA file: " + mcaFile);
			e.printStackTrace();
		}
		
		return region;
	}
	
	public RegionData parse(int regionX, int regionZ, InputStream rawData)
	{
		RegionData region = null;
		try
		{
			region = new RegionData(regionX, regionZ);
			
			byte[] header    = BinaryData.readByteArray(NUM_HEADER_BYTES, rawData);
			byte[] remaining = BinaryData.readAll(rawData);
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
					
					// this means the chunk data is not present (has not been generated or migrated)
					if(num4KiBOffsets == 0 && num4KiBSectors == 0)
					{
						continue;
					}
					
					int chunkDataOffset = (num4KiBOffsets - 2) * 4096;
					int chunkSize       = num4KiBSectors * 4096;
					ByteArrayInputStream chunkData = new ByteArrayInputStream(remaining, chunkDataOffset, chunkSize);
					region.setChunk(chunkX, chunkZ, m_chunkParser.parse(chunkData));
				}
			}
		}
		catch(IOException e)
		{
			System.err.println("error parsing MCA, parsed data may be corrupted");
			e.printStackTrace();
		}
		
		return region;
	}
}
