package plugin.minecraft;

public class ChunkData
{
	public static final int NUM_SECTIONS = 16;
	public static final int SIZE_X       = SectionData.SIZE_X;
	public static final int SIZE_Y       = SectionData.SIZE_Y * NUM_SECTIONS;
	public static final int SIZE_Z       = SectionData.SIZE_Z;
	
	private SectionData[] m_sections;
	
	public ChunkData()
	{
		m_sections = new SectionData[NUM_SECTIONS];
	}
	
	/**
	 * Gets the section data of this chunk. 
	 * @param sectionIndex Index of the section to get, ranging from 0 to 15 (bottom to top).
	 * @return The section data.
	 */
	public SectionData getSection(int sectionIndex)
	{
		return m_sections[sectionIndex];
	}
	
	/**
	 * Sets the section data of this chunk. 
	 * @param sectionIndex Index of the section to set, ranging from 0 to 15 (bottom to top).
	 * @param section Data describing the section.
	 */
	public void setSection(int sectionIndex, SectionData section)
	{
		m_sections[sectionIndex] = section;
	}
	
	@Override
	public String toString()
	{
		int numSectionData = 0;
		for(SectionData data : m_sections)
		{
			if(data != null)
			{
				++numSectionData;
			}
		}
		
		return "Chunk Data, contains " + numSectionData + " section data";
	}
}
