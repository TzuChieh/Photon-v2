package minecraft.parser;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.zip.GZIPInputStream;
import java.util.zip.InflaterInputStream;

import minecraft.ChunkData;
import minecraft.SectionData;
import minecraft.block.StateAggregate;
import minecraft.nbt.NBTData;
import minecraft.nbt.NBTTag;
import util.BinaryData;

public class ChunkParser
{
	private static final int GZIP = 1;
	private static final int ZLIB = 2;
	
	private NBTParser m_nbtParser;
	
	public ChunkParser()
	{
		m_nbtParser = new NBTParser();
	}
	
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
			
			chunk = parseChunkNBT(m_nbtParser.parse(decompressedChunk));
		}
		catch(IOException e)
		{
			System.err.println("error parsing chunk, parsed data may be corrupted");
			e.printStackTrace();
		}
		
		return chunk;
	}
	
	private static ChunkData parseChunkNBT(NBTData data)
	{
		NBTData root  = data.get("");
		NBTData level = root.get("Level");
		
		ChunkData chunkData = new ChunkData();
		
		List<NBTData> sections = level.get("Sections");
		for(NBTData section : sections)
		{
			SectionData chunkSection = new SectionData();
			
			List<NBTData> palette = section.get("Palette");
			for(NBTData block : palette)
			{
				String blockIdName = block.get("Name");
				
				StateAggregate stateProperties = new StateAggregate();
				if(block.hasTag("Properties"))
				{
					NBTData properties = block.get("Properties");
					for(NBTTag property : properties.getTags())
					{
						stateProperties.add(property.getName(), property.getPayload());
					}
				}
				chunkSection.addBlock(blockIdName, stateProperties);
				
				// DEBUG
//				if(blockIdName.contains("fence"))
//				{
//					System.err.println("block id: " + blockIdName);
//					System.err.println("prop: " + stateProperties);
//				}
//				System.err.println("block id: " + blockIdName);
//				System.err.println("prop: " + stateProperties);
			}
			
			long[] blockStates = section.get("BlockStates");
			parseBlockIndices(blockStates, chunkSection.numBlockTypes(), chunkSection);
			
			byte y = section.get("Y");
			chunkData.setSection(y, chunkSection);
		}
		
		return chunkData;
	}
	
	private static void parseBlockIndices(long[] indexData, int numBlockTypes, SectionData out_section)
	{
		final int numIndexBits = Math.max(numBitsNeeded(numBlockTypes - 1), 4);
		
		// Minumum: 4 bits; The maximum should be 12 since each section has at
		// most 16^3=4096 blocks. (if every block is unique, then we will need
		// 4096=2^12 palette entries)
		assert(4 <= numIndexBits && numIndexBits <= 12);
		
		short[][] layer = new short[SectionData.SIZE_Z][SectionData.SIZE_X];
		int bitHead = 0;
		
		for(int y = 0; y < SectionData.SIZE_Y; ++y)
		{
			for(int z = 0; z < SectionData.SIZE_Z; ++z)
			{
				for(int x = 0; x < SectionData.SIZE_X; ++x)
				{
					int dataIndex = bitHead / Long.SIZE;
					int startBit  = bitHead - dataIndex * Long.SIZE;
					int endBit    = startBit + numIndexBits;
					
					short index = 0;
					if(endBit <= Long.SIZE)
					{
						index = (short)((indexData[dataIndex] >> startBit) & bitMask(numIndexBits));
					}
					else
					{
						long data0       = indexData[dataIndex];
						long data1       = indexData[dataIndex + 1];
						int  bitsInData0 = Long.SIZE - startBit;
						int  bitsInData1 = endBit - Long.SIZE;
						
						index |= (short)((data0 >> startBit) & bitMask(bitsInData0));
						index |= (short)((data1 & bitMask(bitsInData1)) << bitsInData0);
					}
					assert(index < numBlockTypes);
					layer[z][x] = index;
					
					bitHead += numIndexBits;
				}
			}
			
			out_section.setLayer(y, layer);
		}
	}
	
	private static int bitMask(int numBits)
	{
		return (1 << numBits) - 1;
	}
	
	private static int numBitsNeeded(int number)
	{
		return Integer.SIZE - Integer.numberOfLeadingZeros(number);
	}
}
