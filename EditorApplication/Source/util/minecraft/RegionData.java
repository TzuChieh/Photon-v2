package util.minecraft;

public class RegionData
{
	public static final int NUM_CHUNKS_X = 32;
	public static final int NUM_CHUNKS_Z = 32;
	
	private int           m_regionX;
	private int           m_regionZ;
	private ChunkData[][] m_chunks;
	
	public RegionData(int regionX, int regionZ)
	{
		m_regionX = regionX;
		m_regionZ = regionZ;
		m_chunks  = new ChunkData[NUM_CHUNKS_Z][NUM_CHUNKS_X];
	}
	
	public int getRegionX()
	{
		return m_regionX;
	}
	
	public int getRegionZ()
	{
		return m_regionZ;
	}
	
	public ChunkData getChunk(int x, int z)
	{
		return m_chunks[z][x];
	}
	
	public void setChunk(int x, int z, ChunkData chunk)
	{
		m_chunks[z][x] = chunk;
	}
	
	@Override
	public String toString()
	{
		int numChunkData = 0;
		for(ChunkData[] chunks : m_chunks)
		{
			for(ChunkData chunk : chunks)
			{
				if(chunk != null)
				{
					++numChunkData;
				}
			}
		}
		
		return "Region Data @ (" + m_regionX + ", " + m_regionZ + "), contains " + numChunkData + " chunk data";
	}
}
