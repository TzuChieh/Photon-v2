package plugin.minecraft;

public class RegionData
{
	public static final int NUM_CHUNKS_X = 32;
	public static final int NUM_CHUNKS_Z = 32;
	public static final int SIZE_X       = ChunkData.SIZE_X * NUM_CHUNKS_X;
	public static final int SIZE_Y       = ChunkData.SIZE_Y;
	public static final int SIZE_Z       = ChunkData.SIZE_Z * NUM_CHUNKS_Z;
	
	private RegionCoord   m_regionCoord;
	private ChunkData[][] m_chunks;
	
	/**
	 * Creates a data block representing a region.
	 * @param regionX The x coordinate of this region, in regions.
	 * @param regionZ The z coordinate of this region, in regions.
	 */
	public RegionData(int regionX, int regionZ)
	{
		m_regionCoord = new RegionCoord(regionX, regionZ);
		m_chunks      = new ChunkData[NUM_CHUNKS_Z][NUM_CHUNKS_X];
	}
	
	/**
	 * Gets the (x, z) coordinate of this region.
	 * @return Coordinate value in regions.
	 */
	public RegionCoord getRegionCoord()
	{
		return m_regionCoord;
	}
	
	public int getX()
	{
		return m_regionCoord.x * SIZE_X;
	}
	
	public int getY()
	{
		return 0;
	}
	
	public int getZ()
	{
		return m_regionCoord.z * SIZE_Z;
	}
	
	public ChunkData getChunk(int chunkX, int chunkZ)
	{
		return m_chunks[chunkZ][chunkX];
	}
	
	public void setChunk(int chunkX, int chunkZ, ChunkData chunk)
	{
		m_chunks[chunkZ][chunkX] = chunk;
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
		
		return "Region Data @ " + m_regionCoord.toString() + ", contains " + numChunkData + " chunk data";
	}
}
