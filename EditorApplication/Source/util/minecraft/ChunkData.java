package util.minecraft;

public class ChunkData
{
	public static final int NUM_SECTIONS = 16;
	
	private int            m_chunkX;
	private int            m_chunkZ;
	private ChunkSection[] m_sections;
	
	/**
	 * Creates a data block representing a chunk.
	 * @param chunkX The x coordinate of this chunk, in chunks.
	 * @param chunkZ The z coordinate of this chunk, in chunks.
	 */
	public ChunkData(int chunkX, int chunkZ)
	{
		m_chunkX   = chunkX;
		m_chunkZ   = chunkZ;
		m_sections = new ChunkSection[NUM_SECTIONS];
	}
	
	/**
	 * Gets the x coordinate of this chunk.
	 * @return Coordinate value in chunks.
	 */
	public int getChunkX()
	{
		return m_chunkX;
	}
	
	/**
	 * Gets the z coordinate of this chunk.
	 * @return Coordinate value in chunks.
	 */
	public int getChunkZ()
	{
		return m_chunkZ;
	}
	
	/**
	 * Gets the section data of this chunk. 
	 * @param sectionIndex Index of the section to get, ranging from 0 to 15 (bottom to top).
	 * @return The section data.
	 */
	public ChunkSection getSection(int sectionIndex)
	{
		return m_sections[sectionIndex];
	}
	
	/**
	 * Sets the section data of this chunk. 
	 * @param sectionIndex Index of the section to set, ranging from 0 to 15 (bottom to top).
	 * @param section Data describing the section.
	 */
	public void setSection(int sectionIndex, ChunkSection section)
	{
		m_sections[sectionIndex] = section;
	}
	
	@Override
	public String toString()
	{
		int numSectionData = 0;
		for(ChunkSection data : m_sections)
		{
			if(data != null)
			{
				++numSectionData;
			}
		}
		
		return "Chunk Data @ (" + m_chunkX + ", " + m_chunkZ + "), contains " + numSectionData + " section data";
	}
}
