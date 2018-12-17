package util.minecraft;

public class RegionData
{
	public static final int NUM_CHUNKS_X = 32;
	public static final int NUM_CHUNKS_Z = 32;
	public static final int SIZE_X       = ChunkData.SIZE_X * NUM_CHUNKS_X;
	public static final int SIZE_Y       = ChunkData.SIZE_Y;
	public static final int SIZE_Z       = ChunkData.SIZE_Z * NUM_CHUNKS_Z;
	
	private int           m_regionX;
	private int           m_regionZ;
	private ChunkData[][] m_chunks;
	
	/**
	 * Creates a data block representing a region.
	 * @param regionX The x coordinate of this region, in regions.
	 * @param regionZ The z coordinate of this region, in regions.
	 */
	public RegionData(int regionX, int regionZ)
	{
		m_regionX = regionX;
		m_regionZ = regionZ;
		m_chunks  = new ChunkData[NUM_CHUNKS_Z][NUM_CHUNKS_X];
	}
	
	/**
	 * Gets the x coordinate of this region.
	 * @return Coordinate value in regions.
	 */
	public int getRegionX()
	{
		return m_regionX;
	}
	
	/**
	 * Gets the z coordinate of this region.
	 * @return Coordinate value in regions.
	 */
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
