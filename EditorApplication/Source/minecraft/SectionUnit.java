package minecraft;

import util.Vector3i;

public class SectionUnit
{
	private Vector3i    m_coord;
	private SectionData m_data;
	
	public SectionUnit(Vector3i coord, SectionData data)
	{
		m_coord = coord;
		m_data  = data;
	}
	
	public Vector3i getCoord()
	{
		return m_coord;
	}
	
	public Vector3i getCenterCoord()
	{
		return m_coord.add(new Vector3i(SectionData.SIZE_X / 2, SectionData.SIZE_Y / 2, SectionData.SIZE_Z / 2));
	}
	
	public SectionData getData()
	{
		return m_data;
	}
	
	@Override
	public String toString()
	{
		return "Section Position: " + m_coord.toString();
	}
}
