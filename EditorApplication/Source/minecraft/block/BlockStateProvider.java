package minecraft.block;

import minecraft.SectionUnit;
import minecraft.Terrain;
import util.Vector3i;

public class BlockStateProvider
{
	private Vector3i m_blockLocalCoord;
	private SectionUnit m_homeSection;
	private Terrain m_neighborSections;
	
	public BlockStateProvider(Vector3i blockLocalCoord, SectionUnit homeSection, Terrain neighborSections)
	{
		m_blockLocalCoord = blockLocalCoord;
		m_homeSection = homeSection;
		m_neighborSections = neighborSections;
	}
	
	public String getBlockId()
	{
		return m_homeSection.getData().getBlockIdName(m_blockLocalCoord.x, m_blockLocalCoord.y, m_blockLocalCoord.z);
	}
	
	public StateProperties getStateProperties(String name)
	{
		return m_homeSection.getData().getProperties(m_blockLocalCoord.x, m_blockLocalCoord.y, m_blockLocalCoord.z);
	}
}
