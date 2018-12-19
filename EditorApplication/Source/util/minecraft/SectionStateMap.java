package util.minecraft;

import java.util.HashMap;
import java.util.Map;

import util.Vector3i;

public class SectionStateMap
{
	private Map<RegionCoord, boolean[][][]> m_regionStates;
	
	public SectionStateMap()
	{
		m_regionStates = new HashMap<>();
	}
	
	public boolean getSection(Vector3i section)
	{
		Vector3i index = toRegionStateIndex(section);
		return getRegionStates(section)[index.y][index.z][index.x];
	}
	
	public void setSection(Vector3i section, boolean state)
	{
		Vector3i index = toRegionStateIndex(section);
		getRegionStates(section)[index.y][index.z][index.x] = state;
	}
	
	private boolean[][][] getRegionStates(Vector3i section)
	{
		return getRegionStates(toRegionCoord(section));
	}
	
	private boolean[][][] getRegionStates(RegionCoord region)
	{
		boolean[][][] bits = m_regionStates.get(region);
		if(bits == null)
		{
			bits = new boolean[ChunkData.NUM_SECTIONS][RegionData.NUM_CHUNKS_Z][RegionData.NUM_CHUNKS_X];
			m_regionStates.put(region, bits);
		}
		return bits;
	}
	
	private static RegionCoord toRegionCoord(Vector3i section)
	{
		return new RegionCoord(
			Math.floorDiv(section.x, RegionData.NUM_CHUNKS_X),
			Math.floorDiv(section.z, RegionData.NUM_CHUNKS_Z));
	}
	
	private static Vector3i toRegionStateIndex(Vector3i section)
	{
		RegionCoord region = toRegionCoord(section);
		
		return new Vector3i(
			section.x - region.x * RegionData.NUM_CHUNKS_X,
			section.y,
			section.z - region.z * RegionData.NUM_CHUNKS_Z);
	}
}
